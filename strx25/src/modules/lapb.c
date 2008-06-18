/*****************************************************************************

 @(#) $RCSfile: lapb.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-06-13 06:43:57 $

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

 Last Modified $Date: 2008-06-13 06:43:57 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: lapb.c,v $
 Revision 0.9.2.1  2008-06-13 06:43:57  brian
 - added files

 *****************************************************************************/

#ident "@(#) $RCSfile: lapb.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-06-13 06:43:57 $"

static char const ident[] = "$RCSfile: lapb.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-06-13 06:43:57 $";

/*
 * This is a LAPB module that pushes over a stream implementing an HDLC
 * communications device.  The upper layer interface is DLPI LAPB and the
 * lower layer interface is CDI HDLC.  The module supports Style 1 and Style 2
 * CDI streams with Style 1 or Style 2 DLPI streams.  Any device driver
 * implementing the CDI HDLC specification can have LAPB support in this way.
 */

#define _MPS_SOURCE

#include <sys/os7/compat.h>
#include <sys/strsun.h>
#include <sys/cdi.h>
#include <sys/dlpi.h>

#define LAPB_DESCRIP	"LAPB OVER HDLC MODULE FOR LINUX FAST-STREAMS"
#define LAPB_EXTRA	"Part of the OpenSS7 X.25 Stack for Linux Fast-STREAMS"
#define LAPB_COPYRIGHT	"Copyright (c) 1997-2008  OpenSS7 Corporation.  All Rights Reserved."
#define LAPB_REVISION	"OpenSS7 $RCSfile: lapb.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-06-13 06:43:57 $"
#define LAPB_DEVICE	"SVR 4.2MP LAPB over HDLC Module (LAPB) for X.25"
#define LAPB_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define LAPB_LICENSE	"GPL"
#define LAPB_BANNER	LAPB_DESCRIP	"\n" \
			LAPB_EXTRA	"\n" \
			LAPB_COPYRIGHT	"\n" \
			LAPB_REVISION	"\n" \
			LAPB_DEVICE	"\n" \
			LAPB_CONTACT	"\n"
#define LAPB_SPLASH	LAPB_DESCRIP	" - " \
			LAPB_REVISION

#ifndef CONFIG_STREAMS_LAPB_NAME
#error "CONFIG_STREAMS_LAPB_NAME must be defined."
#endif				/* CONFIG_STREAMS_LAPB_NAME */
#ifndef CONFIG_STREAMS_LAPB_MODID
#error "CONFIG_STREAMS_LAPB_MODID must be defined."
#endif				/* CONFIG_STREAMS_LAPB_MODID */

#ifdef LINUX
#ifdef MODULE
MODULE_AUTHOR(LAPB_CONTACT);
MODULE_DESCRIPTION(LAPB_DESCRIP);
MODULE_SUPPORTED_DEVICE(LAPB_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(LAPB_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-lapb");
MODULE_ALIAS("streams-module-lapb");
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_LAPB_MODID));
#endif				/* MODULE_ALIAS */
#endif				/* MODULE */
#endif				/* LINUX */

#ifndef LAPB_MOD_NAME
#define LAPB_MOD_NAME		CONFIG_STREAMS_LAPB_NAME
#endif				/* LAPB_MOD_NAME */
#ifndef LAPB_MOD_ID
#define LAPB_MOD_ID		CONFIG_STREAMS_LAPB_MODID
#endif				/* LAPB_MOD_ID */

/*
 * --------------------------------------------------------------------------
 *
 * STREAMS DEFINITIONS
 *
 * --------------------------------------------------------------------------
 */

#define MOD_ID		LAPB_MOD_ID
#define MOD_NAME	LAPB_MOD_NAME
#ifdef MODULE
#define MOD_BANNER	LAPB_BANNER
#else				/* MODULE */
#define MOD_BANNER	LAPB_SPLASH
#endif				/* MODULE */

static struct module_info lapb_minfo = {
	.mi_idnum = MOD_ID,
	.mi_idname = MOD_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_stat lapb_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat lapb_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

/*
 * --------------------------------------------------------------------------
 *
 * PRIVATE STRUCTURE
 *
 * --------------------------------------------------------------------------
 */

struct priv *priv;
struct dl *dl;
struct cd *cd;

/* Upper interface structure. */
struct dl {
	struct priv *priv;
	struct cd *cd;
	queue_t *oq;
	dl_ulong state;
	dl_ulong oldstate;
	dl_ulong coninds;
	dl_ulong oldconinds;
	dl_ulong ppa;
	struct dl *al;
	ushort add_len;
	struct {
		dl_info_ack_t info;
		unsigned char add[20];
		dl_qos_co_sel1_t qos;
		dl_qos_co_range1_t qor;
	} proto;
	ushort loc_len;
	unsigned char loc[20];
	ushort rem_len;
	unsigned char rem[20];
	int mode;			/* Modulo for the connection.  NOTE -- also the number of
					   octets in the head of an I-frame. */
#define DL_MODE_AUTO		 0
#define DL_MODE_BASIC		 1
#define DL_MODE_EXTENDED	 2
#define DL_MODE_SUPER		 4
#define DL_MODE_SUPER_EXTENDED	 8
	int pstate;			/* State of the LAPB protocol SM. */
#define DL_ST_ADM		 0	/* asynchronous disconnected mode */
#define DL_ST_SETUP		 1	/* SABM/SABME/SM sent to set-up link */
#define DL_ST_NORMAL		 2	/* normal data transfer phase */
#define DL_ST_BUSY		 3	/* data transfer with local busy */
#define DL_ST_REJECT		 4	/* reject recovery requested */
#define DL_ST_AWAIT		 5	/* timer recovery */
#define DL_ST_AWAIT_BUSY	 6	/* timer recovery with local busy */
#define DL_ST_AWAIT_REJECT	 7	/* timer recovery with reject requested */
#define DL_ST_D_CONN		 8	/* disconnection pending */
#define DL_ST_RESET		 9	/* user reset pending */
#define DL_ST_ERROR		10	/* FRMR sent, awaiting reply */
#define DL_ST_CONN		11	/* SABM/SABME/SM received to set-up link */
#define DL_ST_RESET_CHECK	12	/* remote provider reset pending */
#define DL_ST_RESET_WAIT	13	/* local provider reset pending */
	uint32_t vs;			/* V(S), next sequence to send */
	uint32_t vr;			/* V(R), next sequence to receive */
	uint32_t nr;			/* N(R), oldest unacknowleged sequence */
	int p_flag;			/* P flag of outstanding command PDU */
	int f_flag;			/* F flag for UA or DM response PDU */
	int s_flag;			/* S flag for oustanding recv SABM/SABME/SM */
	int remote_busy;		/* remote busy flag */
	int txwakeup;			/* trasnmitter wakeup required */
	int sendack;			/* send an acknowledgement RR, RNR, REJ if I frame not
					   available for transmision. */
	int tb_full;			/* the transmission buffer is full */
	int rtb_full;			/* the retransmission buffer is full */
	bufq_t tb;			/* transmission buffer */
	bufq_t rtb;			/* retransmission buffer */
	bufq_t rb;			/* receive buffer */
	struct {
		mblk_t *ack;
		mblk_t *p;
		mblk_t *rej;
		mblk_t *busy;
	} t;
	/* Timer T1: The value for the DTE Timer T1 system parameter may be different form the
	   value for the DCE/remote DTE Timer T1 system parameter.  These values shall be made
	   known to both the DTE and the DCE/remote DTE and agreed to for a designated period of
	   time. The proper operation of the procedure requires that the transmitter's (DTE or
	   DCE/remote DTE) Timer T1 be greater than the maximum time between transmission of frames 
	   (SABM/SABME/SM, DM, DISC, FRMR, I, supervisory commands or REJ response) and the
	   reception of the corresponding frame returned as an answer to this frame (UA, DM or
	   acknowledging frame).  Therefore, the receiver (DTE or DCE/remote DTE) should not delay
	   the response or acknowledging frame returned to the above frames by more than a value
	   T2, where T2 is a system parameter. */
	uint t1;			/* in milliseconds */
	/* Parameter T2: The value of DTE parameter T2 may be different from the value of the
	   DCE/remote DTE parameter T2.  T2 is the amount of time available at the DTE or
	   DCE/remote DTE before the acknolwedging frame must be initiated in order to ensure its
	   receipt by the DCE/remote DTE or DTE, respectively, prior to Timer T1 running out at the 
	   DCE/remote DTE or DTE.  T2 is strictly less than T1. */
	uint t2;			/* milliseconds */
	/* Timer T3: The period of optional Timer T3 shall provide an adequate interval of time to
	   justify considering the data link to be in an disconnected (out of service) state.  T3
	   is much larger than T4. */
	uint t3;			/* milliseconds */
	/* Parameter T4: The parameter T4 is a system parameter which represents the maximum timer
	   a DTE will allow without frames being exchanged on the data link.  The value of T4
	   should be greater than T1 and may be very large for application that are not concerned
	   with early detection of faulty link or physical link conditions.  T4 is much larger than 
	   T1.  */
	uint t4;			/* milliseconds */
	/* Parameter T5: The optional parameter T5 applies only to operation in start/stop
	   environment.  The value of DTE parameter T5 may be different from the value of the
	   DCE/remote DTE parameter T5. T5 is the length of time for which the DTE or DCE/remote
	   DTE will receive a continuous mark condition (logical "1" state) without considering its
	   incoming channel to be in the idle channel state. */
	uint t5;			/* milliseconds */
	/* Maximum number of retransmissions N2: The value of DTE N2 system parameter may be
	   different from the value of the DCE/remote DTE N2 system parameter.  These values shall
	   be made known to both the DTE and the DCE/remote DTE and agreed to for a designated
	   period of time.  */
	uint n2;			/* retries */
	/* Maximum number of octets in an I frame: */
	uint n1;			/* octets */
	/* Maximum number of oustanding frames k: In the case of DTE/DCE operation, the value of
	   the DTE k system parameter shall be the same as the value for the DCE k system
	   parameter.  This value shall be agreed to for a designated period of time by both the
	   DTE and the DCE.  In the case of DTE/remote DTE operation, the value of the DTE k system 
	   parameter may be different from the value of the remote DTE k system parameter.  These
	   values shall be agreed to for a designated period of time by both the DTE and the remote 
	   DTE.  The DTE value of k shall indicate the maximum number of sequentially numbered I
	   frames that the DTE may have oustanding (i.e., unacknowledged) at any given time.  The
	   value of k shall never exceed 7 for modulo 8 operation, or 127 for modulo 128 operation, 
	   or 32,767 for modulo 32,768 operation, or 2,147,483,647 for modulo 2,147,483,648
	   operation.  NOTE -- all DCEs will support a value of seven.  Other values (less than and 
	   greater than seven) may also be supported.

	   The standard window size W is 2 for modulo 8, and 128, and 128 for modulo 32,768, for
	   each direction of data transmission at the DTE/DCE interface. */
	uint k;				/* send window */
};

/* Lower interface structure. */
struct cd {
	struct priv *priv;
	struct dl *dl;
	queue_t *oq;
	cd_ulong state;
	cd_ulong oldstate;
	struct {
		cd_info_ack_t info;
		unsigned char add[20];
	} proto;
};

struct priv {
	struct dl dlpi;
	struct cd cdi;
};

#define PRIV(q) ((struct priv *)q->q_ptr)
#define DL_PRIV(q) (&PRIV(q)->dlpi)
#define CD_PRIV(q) (&PRIV(q)->cdi)

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
static inline const char *
dl_errname(dl_long error)
{
	if (error < 0)
		return ("DL_SYSERR");
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
		return ("(unknown)");
	}
}
static inline const char *
dl_strerror(dl_long error)
{
	if (error < 0)
		return ("UNIX system error occurred.");
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
	default:
		return ("(unknown)");
	}
}

static inline const char *
cd_iocname(int cmd)
{
	switch (cmd) {
	default:
		return ("(unknown)");
	}
}
static inline const char *
cd_primname(cd_ulong prim)
{
	switch (prim) {
	case CD_INFO_REQ:
		return ("CD_INFO_REQ");
	case CD_INFO_ACK:
		return ("CD_INFO_ACK");
	case CD_ATTACH_REQ:
		return ("CD_ATTACH_REQ");
	case CD_DETACH_REQ:
		return ("CD_DETACH_REQ");
	case CD_ENABLE_REQ:
		return ("CD_ENABLE_REQ");
	case CD_DISABLE_REQ:
		return ("CD_DISABLE_REQ");
	case CD_OK_ACK:
		return ("CD_OK_ACK");
	case CD_ERROR_ACK:
		return ("CD_ERROR_ACK");
	case CD_ENABLE_CON:
		return ("CD_ENABLE_CON");
	case CD_DISABLE_CON:
		return ("CD_DISABLE_CON");
	case CD_ERROR_IND:
		return ("CD_ERROR_IND");
	case CD_ALLOW_INPUT_REQ:
		return ("CD_ALLOW_INPUT_REQ");
	case CD_READ_REQ:
		return ("CD_READ_REQ");
	case CD_UNITDATA_REQ:
		return ("CD_UNITDATA_REQ");
	case CD_WRITE_READ_REQ:
		return ("CD_WRITE_READ_REQ");
	case CD_UNITDATA_ACK:
		return ("CD_UNITDATA_ACK");
	case CD_UNITDATA_IND:
		return ("CD_UNITDATA_IND");
	case CD_HALT_INPUT_REQ:
		return ("CD_HALT_INPUT_REQ");
	case CD_ABORT_OUTPUT_REQ:
		return ("CD_ABORT_OUTPUT_REQ");
	case CD_MUX_NAME_REQ:
		return ("CD_MUX_NAME_REQ");
	case CD_BAD_FRAME_IND:
		return ("CD_BAD_FRAME_IND");
	case CD_MODEM_SIG_REQ:
		return ("CD_MODEM_SIG_REQ");
	case CD_MODEM_SIG_IND:
		return ("CD_MODEM_SIG_IND");
	case CD_MODEM_SIG_POLL:
		return ("CD_MODEM_SIG_POLL");
	default:
		return ("(unknown)");
	}
}
static inline const char *
cd_statename(cd_ulong state)
{
	switch (state) {
	case CD_UNATTACHED:
		return ("CD_UNATTACHED");
	case CD_UNUSABLE:
		return ("CD_UNUSABLE");
	case CD_DISABLED:
		return ("CD_DISABLED");
	case CD_ENABLE_PENDING:
		return ("CD_ENABLE_PENDING");
	case CD_ENABLED:
		return ("CD_ENABLED");
	case CD_READ_ACTIVE:
		return ("CD_READ_ACTIVE");
	case CD_INPUT_ALLOWED:
		return ("CD_INPUT_ALLOWED");
	case CD_DISABLE_PENDING:
		return ("CD_DISABLE_PENDING");
	case CD_OUTPUT_ACTIVE:
		return ("CD_OUTPUT_ACTIVE");
	case CD_XRAY:
		return ("CD_XRAY");
	case CD_NOT_AUTH:
		return ("CD_NOT_AUTH");
	default:
		return ("(unknown)");
	}
}
static inline const char *
cd_errname(cd_long error)
{
	if (error < 0)
		return ("CD_SYSERR");
	switch (error) {
	case CD_BADADDRESS:
		return ("CD_BADADDRESS");
	case CD_BADADDRTYPE:
		return ("CD_BADADDRTYPE");
	case CD_BADDIAL:
		return ("CD_BADDIAL");
	case CD_BADDIALTYPE:
		return ("CD_BADDIALTYPE");
	case CD_BADDISPOSAL:
		return ("CD_BADDISPOSAL");
	case CD_BADFRAME:
		return ("CD_BADFRAME");
	case CD_BADPPA:
		return ("CD_BADPPA");
	case CD_BADPRIM:
		return ("CD_BADPRIM");
	case CD_DISC:
		return ("CD_DISC");
	case CD_EVENT:
		return ("CD_EVENT");
	case CD_FATALERR:
		return ("CD_FATALERR");
	case CD_INITFAILED:
		return ("CD_INITFAILED");
	case CD_NOTSUPP:
		return ("CD_NOTSUPP");
	case CD_OUTSTATE:
		return ("CD_OUTSTATE");
	case CD_PROTOSHORT:
		return ("CD_PROTOSHORT");
	case CD_READTIMEOUT:
		return ("CD_READTIMEOUT");
	case CD_SYSERR:
		return ("CD_SYSERR");
	case CD_WRITEFAIL:
		return ("CD_WRITEFAIL");
	default:
		return ("(unknown)");
	}
}
static inline const char *
cd_strerror(cd_long error)
{
	if (error < 0)
		return ("UNIX system error.");
	switch (error) {
	case CD_BADADDRESS:
		return ("Address was invalid.");
	case CD_BADADDRTYPE:
		return ("Invalid address type.");
	case CD_BADDIAL:
		return ("Dial information was invalid.");
	case CD_BADDIALTYPE:
		return ("Invalid dial information type.");
	case CD_BADDISPOSAL:
		return ("Invalid disposal parameter.");
	case CD_BADFRAME:
		return ("Defective SDU received.");
	case CD_BADPPA:
		return ("Invalid PPA identifier.");
	case CD_BADPRIM:
		return ("Unrecognized primitive.");
	case CD_DISC:
		return ("Disconnected.");
	case CD_EVENT:
		return ("Protocol-specific event occurred.");
	case CD_FATALERR:
		return ("Device has become unusable.");
	case CD_INITFAILED:
		return ("Line initialization failed.");
	case CD_NOTSUPP:
		return ("Primitive not supported by this device.");
	case CD_OUTSTATE:
		return ("Primitive was issued from an invalid state.");
	case CD_PROTOSHORT:
		return ("M_PROTO block too short.");
	case CD_READTIMEOUT:
		return ("Read request timed out before data arrived.");
	case CD_SYSERR:
		return ("UNIX system error.");
	case CD_WRITEFAIL:
		return ("Unitdata request failed.");
	default:
		return ("(unknown)");
	}
}

/* Connection service component state descriptions: (from IEEE 802.2 1998) */
static fastcall inline const char *
dl_pstatename(dl_ulong state)
{
	switch (state) {
	case DL_ST_ADM:
		/* ADM: The connection component is in the asynchronous disconnected mode.  It can
		   accept an SABM, SABME or SM PDU from a remote LAPB or, at the request of the
		   service access point user, can initiate an SABM, SABME or SM PDU trasmission to
		   a remote, to establish a data link connection.  it also responds to a DISC
		   command PDU, and to any command PDU with the P bit set to "1".  Normally the
		   DLPI interface will be in the DL_IDLE state. */
		return ("ADM");
	case DL_ST_SETUP:
		/* SETUP: The connection component has transmitted an SABM, SABME, SM command PDU
		   to the remote and is waiting for a reply.  Normally the DLPI interface will be
		   in the DL_OUTCON_PENDING state.  Note that there is an overlap with the D-CONN
		   state. */
		return ("SETUP");
	case DL_ST_NORMAL:
		/* NORMAL: A data link connection exists between the local LAPB service access
		   point and the remote service access point. Sending and reception of information
		   and supervisory PDUs canbe performed.  Normally the DLPI interface will be in
		   the DL_DATAXFER state.  Note that there is an overlap between the NORMAL, BUSY,
		   REJECT, AWAIT, AWAIT-BUSY and AWAIT-REJECT states. */
		return ("NORMAL");
	case DL_ST_BUSY:
		/* BUSY: A data link connection exists between the local LAPB service access point
		   and the remote service access point.  I PDUs may be sent.  Local conditions make 
		   it likely that the information field of received I PDUs will be ignored.
		   Supervisory PDUs may be both sent and received.  Normally the DLPI interface
		   will be in the DL_DATAXFER state.  Note that there is an overlap between the
		   NORMAL, BUSY, REJECT, AWAIT, AWAIT-BUSY and AWAIT-REJECT states. */
		return ("BUSY");
	case DL_ST_REJECT:
		/* REJECT: A data link connection exists between the local LAPB service access
		   point and the remote service access point. The local connection component has
		   requested that the remtoe connection component reset a specific I PDU that the
		   local connection component has detected as being out of sequence. Both I PDUs
		   and supervisory PDUs may be send and received. Normally the DLPI interface will
		   be in the DL_DATAXFER state.  Note that there is an overlap between the NORMAL,
		   BUSY, REJECT, AWAIT, AWAIT-BUSY and AWAIT-REJECT states. */
		return ("REJECT");
	case DL_ST_AWAIT:
		/* AWAIT: A data link connection exists between the local LAPB service access point 
		   and the remote service access point. The local DXE is performing a timer
		   recovery operation and has send a command PDU with the P bit set to "1", and is
		   awaiting an acknowledgement from the remote.  I PDUs may be received but not
		   sent.  Supervisory PDUs may be both send and received. Normally the DLPI
		   interface will be in the DL_DATAXFER state.  Note that there is an overlap
		   between the NORMAL, BUSY, REJECT, AWAIT, AWAIT-BUSY and AWAIT-REJECT states. */
		return ("AWAIT");
	case DL_ST_AWAIT_BUSY:
		/* AWAIT-BUSY: A data link connection exists between the local LAPB service access
		   point adn the remote service access point.  The local DXE is performing a timer
		   recovery operation and has send a command PDU with the P bit set to "1", and is
		   awaiting an acknowledgement from the remote.  I PDUs may not be sent.  Local
		   conditions make it likely that the information field of received I PDUs will be
		   ignored. Supervisory PDUs may be bother send and received.  Normally the DLPI
		   interface will be in the DL_DATAXFER state.  Note that there is an overlap
		   between the NORMAL, BUSY, REJECT, AWAIT, AWAIT-BUSY and AWAIT-REJECT states. */
		return ("AWAIT-BUSY");
	case DL_ST_AWAIT_REJECT:
		/* AWAIT-REJECT: A data link connection exists between the local DXE and the remote 
		   DXE.  The local connection component has requested that the remote connection
		   component re-transmit a specific I PDU that the local connection component has
		   detected as being out of sequence. Before the local DXE entered this state it
		   was performing a timer recovery operation and had sent a command PDU with the P
		   bit set to "1", and is still awaiting an acknowlegement from the remote DXE. I
		   PDUs may be received but not sent. Supervisory PDUs may be both send and
		   received.  Normally the DLPI interface will be in the DL_DATAXFER state.  Note
		   that there is an overlap between the NORMAL, BUSY, REJECT, AWAIT, AWAIT-BUSY and 
		   AWAIT-REJECT states. */
		return ("AWAIT-REJECT");
	case DL_ST_D_CONN:
		/* D-CONN: At the request of the service access point user, the local DXE has sent
		   a DISC command PDPU to the remote DXE DSAP and is waiting for a reply. Normally
		   the DLPI interface will be in the DL_IDLE state.  Note that there is a overlap
		   with the ADM state. */
		return ("D-CONN");
	case DL_ST_RESET:
		/* RESET: As a result of a service access point user request or the receipt of an
		   FRMR response PDU, the local connection component has sent a SABM, SABME, SM
		   command PDU to the remote DXE to reset the data link connection and is waiting
		   for a reply.  Normally the DLPI interface will be in the DL_USER_RESET_PENDING
		   state. */
		return ("RESET");
	case DL_ST_ERROR:
		/* ERROR: The local connection component has detected an error in a received PDU
		   and has sent an FRMR response PDU.  It is waiting for a reply from the remote
		   connection component. Normally the DLPI interface will be in the DL_DATAXFER
		   state. */
		return ("ERROR");
	case DL_ST_CONN:
		/* CONN: The local connection component has received an SABM, SABME, SM PDU from a
		   remote DXE, and is waiting for the local user to accept or refuse the
		   connection.  Normally the DLPI interface will be in DL_INCON_PENDING state. */
		return ("CONN");
	case DL_ST_RESET_CHECK:
		/* RESET-CHECK: The local connection component is waiting fro the local user to
		   accept or refuse a remote reset request. Normally the DLPI interface will be in
		   the DL_PROV_RESET_PENDING state.  Note that there is an overlap with the
		   RESET-WAIT state. */
		return ("RESET-CHECK");
	case DL_ST_RESET_WAIT:
		/* RESET-WAIT: The local connection component is waiting for the local user to
		   indicate a RESET_REQUEST or a DISCONNECT_REQUEST.  Normally the DLPI interface
		   will be in the DL_PROV_RESET_PENDING state.  Note that there is an overlap with
		   the RESET-CHECK state. */
		return ("RESET-WAIT");
	default:
		return ("(invalid)");
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
		mi_strlog(dl->oq, STRLOGST, SL_TRACE, "%s <- %s", dl_statename(newstate),
			  dl_statename(oldstate));
	}
	return (newstate);
}

static dl_ulong
dl_save_state(struct dl *dl)
{
	dl->oldconinds = dl->coninds;
	return ((dl->oldstate = dl_get_state(dl)));
}

static dl_ulong
dl_restore_state(struct dl *dl)
{
	dl->coninds = dl->oldconinds;
	return (dl_set_state(dl, dl->oldstate));
}

static dl_ulong
dl_get_pstate(struct dl *dl)
{
	return (dl->pstate);
}

static dl_ulong
dl_set_pstate(struct dl *dl, dl_ulong newstate)
{
	dl_ulong oldstate = dl->pstate;

	if (newstate != oldstate) {
		dl->pstate = newstate;
		mi_strlog(dl->oq, STRLOGST, SL_TRACE, "%s -> %s", dl_pstatename(oldstate),
			  dl_pstatename(newstate));
	}
	return (newstate);
}

static cd_ulong
cd_get_state(struct cd *cd)
{
	return (cd->state);
}

static cd_ulong
cd_set_state(struct cd *cd, cd_ulong newstate)
{
	cd_ulong oldstate = cd->state;

	if (newstate != oldstate) {
		cd->state = newstate;
		cd->proto.info.cd_current_state = newstate;
		mi_strlog(cd->oq, STRLOGST, SL_TRACE, "%s <- %s", cd_statename(newstate),
			  cd_statename(oldstate));
	}
	return (newstate);
}

static cd_ulong
cd_save_state(struct cd *cd)
{
	cd->oldconinds = cd->coninds;
	return ((cd->oldstate = cd_get_state(cd)));
}

static cd_ulong
cd_restore_state(struct cd *cd)
{
	cd->coninds = cd->oldconinds;
	return (cd_set_state(cd, cd->oldstate));
}

/*
 * --------------------------------------------------------------------------
 *
 * HDLC AND LAPB SENT MESSAGES
 *
 * --------------------------------------------------------------------------
 */

#define LAPB_MT_RR	0x01	/* 0000 0001 */
#define LAPB_MT_RNR	0x05	/* 0000 0101 */
#define LAPB_MT_REJ	0x09	/* 0000 1001 */
#define LAPB_MT_I	0x00	/* 0000 0000 */
#define LAPB_MT_UI	0x03	/* 0000 0011 */
#define LAPB_MT_SABM	0x2f	/* 0010 1111 */
#define LAPB_MT_SABME	0x6f	/* 0110 1111 */
#define LAPB_MT_SM	0xc3	/* 1100 0011 */
#define LAPB_MT_DISC	0x43	/* 0100 0011 */
#define LAPB_MT_DM	0x0f	/* 0000 1111 */
#define LAPB_MT_UA	0x63	/* 0110 0011 */
#define LAPB_MT_FRMR	0x87	/* 1000 0111 */
#define LAPB_MT_XID	0xaf	/* 1010 1111 */
#define LAPB_MT_TEST	0xe3	/* 1110 0011 */

static fastcall noinline __unlikely int
cd_snd_I_frame(struct cd *cd, queue_t *q, mblk_t *msg, int pf, int nr, int ns, mblk_t *dp)
{
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, FASTBUF, BPRI_MED)))) {
		if (likely(canputnext(cd->oq))) {
			*mp->b_wptr++ = cd->caddr;
			if (cd->extended) {
				*mp->b_wptr++ = ns;
				*mp->b_wptr++ = (pf << 7) | nr;
			} else {
				*mp->b_wptr++ = (ns << 4) | (pf << 3) | nr;
			}
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(cd->oq, STRLOGDA, SL_TRACE, "I-Frame ->");
			putnext(cd->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

static fastcall noinline __unlikely int
cd_snd_S_frame(struct cd *cd, queue_t *q, mblk_t *msg, int cmd, int pf, int nr, int mtype)
{
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, FASTBUF, BPRI_MED)))) {
		if (likely(canputnext(cd->oq))) {
			*mp->b_wptr++ = cmd ? cd->caddr : cd->raddr;
			if (cd->extended) {
				*mp->b_wptr++ = mtype;
				*mp->b_wptr++ = (pf << 7) | nr;
			} else {
				*mp->b_wptr++ = mtype | (pf << 3) | nr;
			}
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "S-Frame ->");
			putnext(cd->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

static fastcall noinline __unlikely int
cd_snd_U_frame(struct cd *cd, queue_t *q, mblk_t *msg, int cmd, int pf, int mtype)
{
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, FASTBUF, BPRI_MED)))) {
		if (likely(canputnext(cd->oq))) {
			*mp->b_wptr++ = cmd ? cd->caddr : cd->raddr;
			*mp->b_wptr++ = mtype | (pf << 3);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "U-Frame ->");
			putnext(cd->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/* RR - Receive Ready */
static fastcall noinline __unlikely int
cd_snd_RR(struct cd *cd, queue_t *q, mblk_t *msg, int cmd, int pf, int nr)
{
	mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "RR ->");
	return cd_snd_S_frame(cd, q, msg, cmd, pf, nr, LAPB_MT_RR);
}

/* RNR - Receive Not Ready */
static fastcall noinline __unlikely int
cd_snd_RNR(struct cd *cd, queue_t *q, mblk_t *msg, int cmd, int pf, int nr)
{
	mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "RNR ->");
	return cd_snd_S_frame(cd, q, msg, cmd, pf, nr, LAPB_MT_RNR);
}

/* REJ - Reject */
static fastcall noinline __unlikely int
cd_snd_REJ(struct cd *cd, queue_t *q, mblk_t *msg, int cmd, int pf, int nr)
{
	mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "REJ ->");
	return cd_snd_S_frame(cd, q, msg, cmd, pf, nr, LAPB_MT_REJ);
}

/* SREJ - Selective Reject */
static fastcall noinline __unlikely int
cd_snd_SREJ(struct cd *cd, queue_t *q, mblk_t *msg, int cmd, int pf, int nr)
{
	mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "SREJ ->");
	return cd_snd_S_frame(cd, q, msg, cmd, pf, nr, DL_MT_SREJ);
}

/* UI - Unnumbered Information */
static fastcall noinline __unlikely int
cd_snd_UI(struct cd *cd, queue_t *q, mblk_t *msg, int cmd, int pf, int nr)
{
	mi_strlog(cd->oq, STRLOGDA, SL_TRACE, "UI ->");
	return cd_snd_U_frame(cd, q, msg, 1, pf, LAPB_MT_UI);
}

/* SABM - Set Asyncrhonous Balanced Mode (modulo 8) */
static fastcall noinline __unlikely int
cd_snd_SABM(struct cd *cd, queue_t *q, mblk_t *msg, int p)
{
	mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "SABM ->");
	return cd_snd_U_frame(cd, q, msg, 1, p, LAPB_MT_SABM);
}

/* SABME - Set Asyncrhonous Balanced Mode Extended (modulo 127) */
static fastcall noinline __unlikely int
cd_snd_SABME(struct cd *cd, queue_t *q, mblk_t *msg, int p)
{
	mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "SABME ->");
	return cd_snd_U_frame(cd, q, msg, 1, p, LAPB_MT_SABME);
}

/* SM - Set Mode (modulo 32,768) */
cd_snd_SM(struct cd * cd, queue_t *q, mblk_t *msg, int p)
{
	switch (cd->dl->mode) {
	default:
	case DL_MODE_AUTO:
		mi_strlog(cd->oq, STRLOGTX, SL_TRACE < "DM ->");
		return cd_snd_U_frame(cd, q, msg, 0, 0, LAPB_MT_DM);
	case DL_MODE_BASIC:
		mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "SABM ->");
		return cd_snd_U_frame(cd, q, msg, 1, p, LAPB_MT_SABM);
	case DL_MODE_EXTENDED:
		mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "SABME ->");
		return cd_snd_U_frame(cd, q, msg, 1, p, LAPB_MT_SABME);
	case DL_MODE_SUPER:
		mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "SM ->");
		return cd_snd_U_frame(cd, q, msg, 1, p, LAPB_MT_SM);
	case DL_MODE_SUPER_EXTENDED:
		mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "SME ->");
		return cd_snd_U_frame(cd, q, msg, 1, p, DL_MT_SME);
	}
}

/* SME - Set Mode Extended (modulo 2,147,483,648) */
cd_snd_SME(struct cd * cd, queue_t *q, mblk_t *msg, int p)
{
	mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "SME ->");
	return cd_snd_U_frame(cd, q, msg, 1, p, DL_MT_SME);
}

/* DISC - Disconnect */
static fastcall noinline __unlikely int
cd_snd_DISC(struct cd *cd, queue_t *q, mblk_t *msg, int p)
{
	mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "DISC ->");
	return cd_snd_U_frame(cd, q, msg, 1, p, LAPB_MT_DISC);
}

/* DM - Disconnect Mode */
static fastcall noinline __unlikely int
cd_snd_DM(struct cd *cd, queue_t *q, mblk_t *msg, int f)
{
	mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "DM ->");
	return cd_snd_U_frame(cd, q, msg, 0, f, LAPB_MT_DM);
}

/* UA - Unnumbered Acknowledgement */
static fastcall noinline __unlikely int
cd_snd_UA(struct cd *cd, queue_t *q, mblk_t *msg, int f)
{
	mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "UA ->");
	return cd_snd_U_frame(cd, q, msg, 0, f, LAPB_MT_UA);
}

/* FRMR - Frame Reject */
static fastcall noinline __unlikely int
cd_snd_FRMR(struct cd *cd, queue_t *q, mblk_t *msg, uchar *rfcf, int pf, int vs, int cr,
	    int vr, int flags)
{
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, 18, BPRI_MED)))) {
		if (likely(canputnext(cd->oq))) {
			*mp->b_wptr++ = (pf << 4) | LAPB_MT_FRMR;
			switch (cd->dl->mode) {
			case DL_MODE_AUTO:
			case DL_MODE_BASIC:
				*mp->b_wptr++ = rfcf[0];
				*mp->b_wptr++ = (vs << 1) | (cr << 4) | (vr << 5);
				*mp->b_wptr++ = flags;
				break;
			case DL_MODE_EXTENDED:
				*mp->b_wptr++ = rfcf[0];
				*mp->b_wptr++ = rfcf[1];
				*mp->b_wptr++ = (vs << 1) | 0;
				*mp->b_wptr++ = (vr << 1) | cr;
				break;
			case DL_MODE_SUPER:
				*mp->b_wptr++ = rfcf[0];
				*mp->b_wptr++ = rfcf[1];
				*mp->b_wptr++ = rfcf[2];
				*mp->b_wptr++ = rfcf[3];
				*mp->b_wptr++ = (vs << 1);
				*mp->b_wptr++ = (vs >> 7);
				*mp->b_wptr++ = (vr << 1) | cr;
				*mp->b_wptr++ = (vr >> 7);
				break;
			case DL_MODE_SUPER_EXENTDED:
				*mp->b_wptr++ = rfcf[0];
				*mp->b_wptr++ = rfcf[1];
				*mp->b_wptr++ = rfcf[2];
				*mp->b_wptr++ = rfcf[3];
				*mp->b_wptr++ = rfcf[4];
				*mp->b_wptr++ = rfcf[5];
				*mp->b_wptr++ = rfcf[6];
				*mp->b_wptr++ = rfcf[7];
				*mp->b_wptr++ = (vs << 1);
				*mp->b_wptr++ = (vs >> 7);
				*mp->b_wptr++ = (vs >> 15);
				*mp->b_wptr++ = (vs >> 23);
				*mp->b_wptr++ = (vr << 1) | cr;
				*mp->b_wptr++ = (vr >> 7);
				*mp->b_wptr++ = (vr >> 15);
				*mp->b_wptr++ = (vr >> 23);
				break;
			}
			*mp->b_wptr++ = flags;
			freemsg(msg);
			mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "FRMR ->");
			putnext(cd->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/* I - Information */
static fastcall noinline __unlikely int
cd_snd_I(struct cd *cd, queue_t *q, mblk_t *msg, int pf, int nr, int ns, mblk_t *dp)
{
	mi_strlog(cd->oq, STRLOGDA, SL_TRACE, "I ->");
	return cd_snd_I_frame(cd, q, msg, pf, nr, ns, dp);
}

/*
 * --------------------------------------------------------------------------
 *
 * DLPI PROVIDER TO DLPI USER ISSUED PRIMITIVES
 *
 * --------------------------------------------------------------------------
 */

static fastcall noinline __unlikely int
m_error(struct dl *dl, queue_t *q, mblk_t *msg, int rerr, int werr)
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

/**
 * m_error_reply: reply with M_ERROR message when necessary
 * @dl: data link private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @error: negative UNIX error, positive DLPI error
 *
 * This function is like m_error() except that it passes through error codes that the caller
 * should simply return without generating an M_ERROR, and also, it translates positive DLPI
 * errors into EPROTO.
 */
static fastcall noinline __unlikely int
m_error_reply(struct dl *dl, queue_t *q, mblk_t *msg, int error)
{
	switch (error) {
	case 0:
	case -ENOBUFS:
	case -EAGAIN:
	case -EDEADLK:
	case -EBUSY:
	case -ENOMEM:
		return (error);
	}
	if (error > 0)
		return m_error(dl, q, msg, EPROTO, EPROTO);
	return m_error(dl, q, msg, -error, -error);
}

static fastcall noinline __unlikely int
dl_info_ack(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_info_ack_t *p;
	mblk_t *mp;
	size_t add_len = dl->proto.info.add_len;
	size_t qos_len = sizeof(dl->proto.qos);
	size_t qor_len = sizeof(dl->proto.qor);
	size_t brd_len = dl->proto.info.brd_len;
	size_t msg_len = sizeof(*p) + qos_len + qor_len + add_len + brd_len;

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
		p->dl_qos_offset = sizeof(*p);
		p->dl_qos_range_length = qor_len;
		p->dl_qos_range_offset = p->dl_qos_offset + qos_len;
		p->dl_provider_style = dl->proto.info.dl_provider_style;
		p->dl_addr_offset = p->dl_qos_range_offset + qor_len;
		p->dl_version = dl->proto.info.dl_version;
		p->dl_brdcst_addr_length = brd_len;
		p->dl_brdcst_addr_offset = p->dl_addr_offset + add_len;
		p->dl_growth = 0;
		mp->b_wptr += sizeof(*p);
		bcopy(&dl->proto.qos, mp->b_wptr, qos_len);
		mp->b_wptr += qos_len;
		bcopy(&dl->proto.qor, mp->b_wptr, qor_len);
		mp->b_wptr += qor_len;
		bcopy(&dl->proto.add, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
		bcopy(&dl->proto.brd, mp->b_wptr, brd_len);
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
	mblk_t *mp;
	size_t add_len = dl->proto.info.dl_addr_length;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p) + add_len, BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_BIND_ACK;
		p->dl_sap = dl->proto.info.dl_sap;
		p->dl_addr_length = add_len;
		p->dl_addr_offset = sizeof(*p);
		mp->b_wptr += sizeof(*p);
		bcopy(dl->proto.add, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
		freemsg(msg);
		dl_set_state(dl, DL_IDLE);
		mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_BIND_ACK");
		putnext(dl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
dl_ok_ack(struct dl *dl, queue_t *q, mblk_t *msg, dl_ulong prim)
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
			freemsg(msg);
			dl_set_state(dl, DL_UNBOUND);
			break;
		case DL_DETACH_PENDING:
			freemsg(msg);
			dl_set_state(dl, DL_UNATTACHED);
			break;
		case DL_UNBIND_PENDING:
			freemsg(msg);
			dl_set_state(dl, DL_UNBOUND);
			break;
		case DL_UDQOS_PENDING:
			freemsg(msg);
			dl_set_state(dl, DL_IDLE);
			break;
		case DL_CONN_RES_PENDING:
			switch (dl_get_pstate(dl)) {
			case DL_ST_CONN:
				if ((err = cd_snd_UA(dl->cd, q, msg, dl->f_flag)))
					goto error;
				dl->vs = 0;
				dl->vr = 0;
				dl->retry_count = 0;
				dl->p_flag = 0;
				dl->remote_busy = 0;
				dl_set_pstate(dl, DL_ST_NORMAL);
				break;
			default:
				err = -EPROTO;
				goto error;
			}
			dl_set_state(dl, DL_DATAXFER);
			break;
		case DL_RESET_RES_PENDING:
			switch (dl_get_pstate(dl)) {
			case DL_ST_RESET_WAIT:
				/* local provider reset */
				if (dl->s_flag == 0) {
					if ((err = cd_snd_SM(dl->cd, q, msg, 1)))
						goto error;
					dl->p_flag = 1;
					dl->retry_count = 0;
					mi_timer(dl->t.ack, dl->t1);
					dl_set_pstate(dl, DL_ST_RESET);
				} else {
					if ((err = cd_snd_UA(dl->cd, q, msg, dl->f_flag)))
						goto error;
					dl->vs = 0;
					dl->vr = 0;
					dl->retry_count = 0;
					dl->p_flag = 0;
					dl->remote_busy = 0;
					dl_set_pstate(dl, DL_ST_NORMAL);
				}
				break;
			case DL_ST_RESET_CHECK:
				/* remote provider reset */
				if ((err = cd_snd_UA(dl->cq, q, msg, dl->f_flag)))
					goto error;
				dl->vs = 0;
				dl->vr = 0;
				dl->retry_count = 0;
				dl->p_flag = 0;
				dl->remote_busy = 0;
				dl_set_pstate(dl, DL_ST_NORMAL);
				break;
			default:
				err = -EPROTO;
				goto error;
			}
			dl_set_state(dl, DL_DATAXFER);
			break;
		case DL_DISCON8_PENDING:
			switch (dl_get_pstate(dl)) {
			case DL_ST_SETUP:
				/* setup still in progress */
				/* this is not in IEEE 802.2 state machines! */
				if ((err = cd_snd_DM(dl->cd, q, msg, 1)))
					goto error;
				mi_timer_stop(dl->t.ack);
				dl_set_pstate(dl, DL_ST_ADM);
				break;
			default:
				err = -EPROTO;
				goto error;
			}
			dl_set_state(dl, DL_IDLE);
			break;
		case DL_DISCON9_PENDING:
			switch (dl_get_pstate(dl)) {
			case DL_ST_CONN:
				if ((err = cd_snd_DM(dl->cd, q, msg, dl->f_flag)))
					goto error;
				dl_set_pstate(dl, DL_ST_ADM);
				break;
			default:
				err = -EPROTO;
				goto error;
			}
			dl_set_state(dl, DL_IDLE);
			break;
		case DL_DISCON11_PENDING:
			switch (dl_get_pstate(dl)) {
			case DL_ST_NORMAL:
			case DL_ST_BUSY:
			case DL_ST_REJECT:
			case DL_ST_AWAIT:
			case DL_ST_AWAIT_BUSY:
			case DL_ST_AWAIT_REJECT:
				if ((err = cd_snd_DISC(dl->cd, q, msg, 1)))
					goto error;
				dl->p_flag = 1;
				dl->retry_count = 0;
				mi_timer_stop(dl->t.ack);
				mi_timer_stop(dl->t.p);
				mi_timer_stop(dl->t.rej);
				mi_timer_stop(dl->t.busy);
				mi_timer(dl->t.ack, dl->t1);
				dl_set_pstate(dl, DL_ST_D_CONN);
				break;
			default:
				err = -EPROTO;
				goto error;
			}
			dl_set_state(dl, DL_IDLE);
			break;
		case DL_DISCON12_PENDING:
			/* FIXME: from what state? */
			/* send DISC command with P bit set */
			if ((err = cd_snd_DISC(dl->cd, q, msg, 1)))
				goto error;
			dl_set_state(dl, DL_IDLE);
			break;
		case DL_DISCON13_PENDING:
			switch (dl_get_pstate(dl)) {
			case DL_ST_RESET_WAIT:
				/* local provider reset */
				if (dl->s_flag == 0) {
					if ((err = cd_snd_DISC(dl->cd, q, msg, 1)))
						goto error;
					dl->p_flag = 1;
					dl->retry_count = 0;
					mi_timer(dl->t.ack, dl->t1);
					dl_set_pstate(dl, DL_ST_D_CONN);
				} else {
					if ((err = cd_snd_DM(dl->cd, q, msg, dl->f_flag)))
						goto error;
					dl_set_pstate(dl, DL_ST_ADM);
				}
				break;
			case DL_ST_RESET_CHECK:
				/* remote provider reset */
				if ((err = cd_snd_DM(dl->cd, q, msg, dl->f_flag)))
					goto error;
				dl_set_pstate(dl, DL_ST_ADM);
				break;
			default:
				err = -EPROTO;
				goto error;
			}
			dl_set_state(dl, DL_IDLE);
			break;
		}
		mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_OK_ACK");
		putnext(dl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
      error:
	freeb(mp);
	return (err);
}
static fastcall noinline __unlikely int
dl_error_ack(struct dl *dl, queue_t *q, mblk_t *msg, dl_ulong prim, dl_long error)
{
	dl_error_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_ERROR_ACK;
		p->dl_error_primitive = prim;
		p->dl_errno = error < 0 ? DL_SYSERR : 0;
		p->dl_unix_errno = error < 0 ? -error : 0;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		switch (dl_get_state(dl)) {
		case DL_ATTACH_PENDING:
			dl_set_state(dl, DL_UNATTACHED);
			break;
		case DL_DETACH_PENDING:
			dl_set_state(dl, DL_UNBOUND);
			break;
		case DL_BIND_PENDING:
			dl_set_state(dl, DL_UNBOUND);
			break;
		case DL_UNBIND_PENDING:
			dl_set_state(dl, DL_IDLE);
			break;
		case DL_UDQOS_PENDING:
			dl_set_state(dl, DL_IDLE);
			break;
		case DL_OUTCON_PENDING:
			dl_set_state(dl, DL_IDLE);
			break;
		case DL_CONN_RES_PENDING:
			dl_set_state(dl, DL_INCON_PENDING);
			break;
		case DL_RESET_RES_PENDING:
			dl_set_state(dl, DL_PROV_RESET_PENDING);
			break;
		case DL_USER_RESET_PENDING:
			dl_set_state(dl, DL_DATAXFER);
			break;
		case DL_DISCON8_PENDING:
			dl_set_state(dl, DL_OUTCON_PENDING);
			break;
		case DL_DISCON9_PENDING:
			dl_set_state(dl, DL_INCON_PENDING);
			break;
		case DL_DISCON11_PENDING:
			dl_set_state(dl, DL_DATAXFER);
			break;
		case DL_DISCON12_PENDING:
			dl_set_state(dl, DL_USER_RESET_PENDING);
			break;
		case DL_DISCON13_PENDING:
			dl_set_state(dl, DL_PROV_RESET_PENDING);
			break;
		case DL_SUBS_BIND_PND:
			dl_set_state(dl, DL_IDLE);
			break;
		case DL_SUBS_UNBIND_PND:
			dl_set_state(dl, DL_IDLE);
			break;
		}
		mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_ERROR_ACK");
		putnext(dl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
dl_subs_bind_ack(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_subs_bind_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(dl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_SUBS_BIND_ACK;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			dl_set_state(dl, dl_get_state(dl));
			mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_SUBS_BIND_ACK");
			putnext(dl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
dl_unitdata_ind(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_unitdata_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(dl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_UNITDATA_IND;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			dl_set_state(dl, dl_get_state(dl));
			mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_UNITDATA_IND");
			putnext(dl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
dl_uderror_ind(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_uderror_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(dl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_UDERROR_IND;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			dl_set_state(dl, dl_get_state(dl));
			mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_UDERROR_IND");
			putnext(dl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * dl_connect_ind: - issue DL_CONNECT_IND primitive
 * @dl: data link private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 *
 * The called address, calling address, and responding address parameters of the DL-CONNECT
 * primitives are associated a priori with the DTE and the DCE or remote DTE at the two ends of the
 * point-to-point data link, and hence are not mapped in the protocol.
 *
 * Similarly, the Quality of Service parameter set parameters are not mapped in the protocol, since
 * only one level of QOS is available and is assumed known a priori.
 */
static fastcall noinline __unlikely int
dl_connect_ind(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_connect_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(dl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_CONNECT_IND;
			p->dl_correlation = ++dl->correlation;
			p->dl_called_addr_length = 0;
			p->dl_called_addr_offset = 0;
			p->dl_calling_addr_length = 0;
			p->dl_calling_addr_offset = 0;
			p->dl_qos_length = 0;
			p->dl_qos_offset = 0;
			p->dl_growth = 0;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
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

/**
 * dl_connect_con: - issued DL_CONNECT_CON primitive
 * @dl: data link private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 *
 * The called address, calling address and responding address parameters of the DL-CONNECT
 * primitives are associated a priori with the DTE and the DCE or remote DTE at the two ends of the
 * point-to-point data link, and hence are not mapped in the protocol.
 *
 * Similarly, the Quality of service parameter set are not mapped in the protocol, since only one
 * level of QOS is available and is assumed a priori.
 *
 * - UA response received for SABM/SABM/SM/SME command (re)transmitted in disconnected mode.
 */
static fastcall noinline __unlikely int
dl_connect_con(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_connect_con_t *p;
	mblk_t *mp;
	size_t qos_len = sizeof(dl->proto.info.qos);

	if (likely(!!(mp = mi_allocb(q, sizeof(*p) + qos_len, BPRI_MED)))) {
		if (likely(canputnext(dl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_CONNECT_CON;
			p->dl_resp_addr_length = 0;
			p->dl_resp_addr_offset = 0;
			p->dl_qos_length = qos_len;
			p->dl_qos_offset = sizeof(*p);
			p->dl_growth = 0;
			mp->b_wptr += sizeof(*p);
			bcopy(&dl->proto.info.qos, mp->b_wptr, qos_len);
			mp->b_wptr += qos_len;
			freemsg(msg);
			dl_set_state(dl, DL_DATAXFER);
			mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_CONNECT_CON");
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
		if (likely(canputnext(dl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_TOKEN_ACK;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			dl_set_state(dl, dl_get_state(dl));
			mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_TOKEN_ACK");
			putnext(dl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

static fastcall inline __hot_read int
dl_data_ind(struct dl *dl, queue_t *q, mblk_t *msg, mblk_t *mp)
{
	if (likely(canpuntext(dl->oq))) {
		/* strip the I frame header */
		mp->b_rptr += dl->mode;
		if (msg && msg->b_cont == mp)
			msg->b_cont = NULL;
		freemsg(msg);
		/* Spec says not to indicate zero length frames. */
		if (msgdsize(mp) > 0) {
			putnext(dl->oq, mp);
			return (0);
		}
		freemsg(mp);
		return (0);
	}
	/* XXX: indicate busy */
	return (-EBUSY)
}

/**
 * dl_disconnect_ind: - issue DL_DISCONNECT_IND primitive
 * @dl: data link private structure
 * @q: active queue
 * @msg: message to free upon sucess (or NULL)
 *
 * The Originator parameter in a DL-DISCONNECT indication primitive is "DLS provider" if the
 * primitive corresponds to a DM frame received in data transfer phase, and otherwise "unknown".
 *
 * The Reason parameter in every DL-DISCONNECT request and indication primitive is "reason
 * unspecified".
 *
 * Note, however, if the DLS provider is locally responding with a DL_DISCONNECT_IND that does not
 * involve the X.25 protocol (e.g. routing procedures, QOS negotiation), then the originator and
 * reason could be different.
 *
 * - DISC command or DM-response received when in information transfer phase.
 *
 * - DM-response received for SABM/SABME/SM command (re)transmitted in diconnected mode (rejection
 *   of DLC establishment).
 *
 * - DM-response transmitted during information transfer phase (in response to received FRMR or
 *   unsolicited UA response, or to unsolicited response from with F bit set to 1), together with
 *   any retransmissions on timer expiry.
 *
 * - Entry to disconnected mode on retransmission-count expiry during information transfer phase or
 *   link set-up.
 *
 * - Detection of loss of physical layer communication.
 */
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
			dl_set_state(dl, dl_get_state(dl));
			mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_DISCONNECT_IND");
			putnext(dl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * dl_reset_ind: - issued DL_RESET_IND primitive
 * @dl: data link private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 *
 * SABM/SABME/SM command received (Note 1).
 *
 * SABM/SABME/SM command transmitted on receiving unsolicited responsed frame with F bit set to 1
 * (Note 1).
 *
 * FRMR response received (Note 1).
 *
 * FRMR response transmitted on entry to frame rejection exception condition (Note 1).
 *
 * Note 1: The first occurence of one of these frames during normal data transfer, together with any
 * retransmissions required by the X.25 LAPB procedures, maps to the DL-RESET request or indication
 * primitive.  Subsequent occurences of other frames from this set before either the link reset is
 * completed or the link is disconnected do not map to any DLS primitive.
 */
static fastcall noinline __unlikely int
dl_reset_ind(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_reset_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(dl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_RESET_IND;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			dl_set_state(dl, dl_get_state(dl));
			mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_RESET_IND");
			putnext(dl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * dl_reset_con: - issued a DL_RESET_CON primitive
 * @dl: data link private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 *
 * - Following a DL-RESET request same mapping as for DL-RESET response.
 *
 * - UA response transmitted or received, as appropriate, to complete a link reset.
 *
 * - Time-out waiting for UA response, after sending UA response to a colliding SABM/SABME/SM
 *   command received.
 *
 * The correspondence between these primitives, marking completion of DLC resetting, and the
 * protocol frames or timeouts uses the earliest externally observable real-world events with which
 * the abstract primitives can be associated.  The significance of the primitives in the CO-DLS is
 * that they separate a period when DL-DATA primitive cannot occur from the following period when
 * DL-DATA primitives are again possible: the mapping specified simply relates this to the
 * equivalent separation between X.25 LAPB link resetting procedure, during which no information
 * transfer occurs, and the resumption of the capability for normal information transfer on
 * completion of the link reset.  Within an implementation, it may be convenient to consider
 * representations of the primitives as occurring either earlier or later.  If earlier, there will
 * be a period during which DL-DATA request primitives as occurring either earlier or later.  If
 * earlier, there will be a period during which DL-DATA request primitives cannot be issued at the
 * DLSAP, since the X.25 LAPB procedures prevent transmission of I-frames; if later, there will be a
 * period following completion of the X.25 LAPB link reset during which I-frames are not
 * transmitted, because the local implementation is not ready.  Such an implementation-based view is
 * not precluded, since it is outside the scope of OSI standardization.
 */
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
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			dl_set_state(dl, dl_get_state(dl));
			mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_RESET_CON");
			putnext(dl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
dl_data_ack_ind(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_data_ack_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(dl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_DATA_ACK_IND;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			dl_set_state(dl, dl_get_state(dl));
			mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_DATA_ACK_IND");
			putnext(dl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
dl_data_ack_status_ind(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_data_ack_status_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(dl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_DATA_ACK_STATUS_IND;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			dl_set_state(dl, dl_get_state(dl));
			mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_DATA_ACK_STATUS_IND");
			putnext(dl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
dl_reply_ind(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_reply_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(dl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_REPLY_IND;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			dl_set_state(dl, dl_get_state(dl));
			mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_REPLY_IND");
			putnext(dl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
dl_reply_status_ind(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_reply_status_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(dl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_REPLY_STATUS_IND;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			dl_set_state(dl, dl_get_state(dl));
			mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_REPLY_STATUS_IND");
			putnext(dl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
dl_reply_update_status_ind(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_reply_update_status_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(dl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_REPLY_UPDATE_STATUS_IND;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			dl_set_state(dl, dl_get_state(dl));
			mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_REPLY_UPDATE_STATUS_IND");
			putnext(dl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
dl_xid_ind(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_xid_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(dl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_XID_IND;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			dl_set_state(dl, dl_get_state(dl));
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
dl_xid_con(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_xid_con_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(dl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_XID_CON;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			dl_set_state(dl, dl_get_state(dl));
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
dl_test_ind(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_test_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(dl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_TEST_IND;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			dl_set_state(dl, dl_get_state(dl));
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
dl_test_con(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_test_con_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(dl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_TEST_CON;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			dl_set_state(dl, dl_get_state(dl));
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
dl_phys_addr_ack(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_phys_addr_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(dl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_PHYS_ADDR_ACK;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			dl_set_state(dl, dl_get_state(dl));
			mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_PHYS_ADDR_ACK");
			putnext(dl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
dl_get_statistics_ack(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_get_statistics_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(dl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_GET_STATISTICS_ACK;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			dl_set_state(dl, dl_get_state(dl));
			mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_GET_STATISTICS_ACK");
			putnext(dl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 * --------------------------------------------------------------------------
 *
 * CDI USER TO CDI PROVIDER ISSUED PRIMITIVES
 *
 * --------------------------------------------------------------------------
 */
static fastcall noinline __unlikely int
cd_info_req(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_info_req_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(cd->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->cd_primitive = CD_INFO_REQ;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			cd_set_state(cd, cd_get_state(cd));
			mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "CD_INFO_REQ ->");
			putnext(cd->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
cd_attach_req(struct cd *cd, queue_t *q, mblk_t *msg, cd_ulong ppa)
{
	cd_attach_req_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(cd->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->cd_primitive = CD_ATTACH_REQ;
			p->cd_ppa = ppa;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			cd_set_state(cd, CD_ATTACH_PENDING);
			mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "CD_ATTACH_REQ ->");
			putnext(cd->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
cd_detach_req(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_detach_req_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->cd_primitive = CD_DETACH_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		cd_set_state(cd, CD_DETACH_PENDING);
		mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "CD_DETACH_REQ ->");
		putnext(cd->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
cd_enable_req(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_enable_req_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(cd->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->cd_primitive = CD_ENABLE_REQ;
			p->cd_dial_type = CD_NODIAL;
			p->cd_dial_length = 0;
			p->cd_dial_offset = sizeof(*p);
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			cd_set_state(cd, CD_ENABLE_PENDING);
			mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "CD_ENABLE_REQ ->");
			putnext(cd->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
cd_disable_req(struct cd *cd, queue_t *q, mblk_t *msg, cd_ulong disposal)
{
	cd_disable_req_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(cd->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->cd_primitive = CD_DISABLE_REQ;
			p->cd_disposal = disposal;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			cd_set_state(cd, CD_DISABLE_PENDING);
			mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "CD_DISABLE_REQ ->");
			putnext(cd->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
cd_allow_input_req(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_allow_input_req_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(cd->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->cd_primitive = CD_ALLOW_INPUT_REQ;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			cd_set_state(cd, cd_get_state(cd));
			mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "CD_ALLOW_INPUT_REQ ->");
			putnext(cd->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
cd_read_req(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_read_req_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(cd->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->cd_primitive = CD_READ_REQ;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			cd_set_state(cd, cd_get_state(cd));
			mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "CD_READ_REQ ->");
			putnext(cd->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
cd_unitdata_req(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_unitdata_req_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(cd->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->cd_primitive = CD_UNITDATA_REQ;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			cd_set_state(cd, cd_get_state(cd));
			mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "CD_UNITDATA_REQ ->");
			putnext(cd->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
cd_write_read_req(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_write_read_req_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(cd->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->cd_primitive = CD_WRITE_READ_REQ;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			cd_set_state(cd, cd_get_state(cd));
			mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "CD_WRITE_READ_REQ ->");
			putnext(cd->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
cd_halt_input_req(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_halt_input_req_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(cd->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->cd_primitive = CD_HALT_INPUT_REQ;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			cd_set_state(cd, cd_get_state(cd));
			mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "CD_HALT_INPUT_REQ ->");
			putnext(cd->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
cd_abort_output_req(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_abort_output_req_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(cd->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->cd_primitive = CD_ABORT_OUTPUT_REQ;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			cd_set_state(cd, cd_get_state(cd));
			mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "CD_ABORT_OUTPUT_REQ ->");
			putnext(cd->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
cd_mux_name_req(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_mux_name_req_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(cd->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->cd_primitive = CD_MUX_NAME_REQ;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			cd_set_state(cd, cd_get_state(cd));
			mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "CD_MUX_NAME_REQ ->");
			putnext(cd->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
cd_modem_sig_req(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_modem_sig_req_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(cd->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->cd_primitive = CD_MODEM_SIG_REQ;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			cd_set_state(cd, cd_get_state(cd));
			mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "CD_MODEM_SIG_REQ ->");
			putnext(cd->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
cd_modem_sig_poll(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_modem_sig_poll_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(cd->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->cd_primitive = CD_MODEM_SIG_POLL;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			cd_set_state(cd, cd_get_state(cd));
			mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "CD_MODEM_SIG_POLL ->");
			putnext(cd->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 * --------------------------------------------------------------------------
 *
 * LAPB RECEIVED MESSAGES
 *
 * --------------------------------------------------------------------------
 */

/* Connection service component event descriptions: (from IEEE 802.2 1998) */
#define DL_E_CONNECT_REQUEST			 1
#define DL_E_CONNECT_RESPONSE			 2
#define DL_E_DATA_REQUEST			 3
#define DL_E_RESET_REQUEST			 4
#define DL_E_RESET_RESPONSE			 5
#define DL_E_LOCAL_BUSY_DETECTED		 6
#define DL_E_LOCAL_BUSY_CLEARED			 7
#define DL_E_RECEIVE_BAD_PDU			 8
#define DL_E_RECEIVE_DISC_CMD			 9
#define DL_E_RECEIVE_DM_RSP			10
#define DL_E_RECEIVE_FRMR_RSP			11
#define DL_E_RECEIVE_I_CMD			12
#define DL_E_RECEVIE_I_CMD_UNEXP_NS		13
#define DL_E_RECEIVE_I_CMD_INVALID_NS		14
#define DL_E_RECEVIE_I_CMD_DUPLICATE		15
#define DL_E_RECEVIE_I_RSP			16
#define DL_E_RECEIVE_I_RSP_UNEXP_NS		17
#define DL_E_RECEIVE_I_RSP_INVALID_NS		18
#define DL_E_RECEIVE_I_RSP_DUPLICATE		19
#define DL_E_RECEVIE_REJ_CMD			20
#define DL_E_RECEIVE_REJ_RSP			21
#define DL_E_RECEIVE_RNR_CMD			22
#define DL_E_RECEIVE_RNR_RSP			23
#define DL_E_RECEIVE_RR_CMD			24
#define DL_E_RECEIVE_RR_RSP			25
#define DL_E_RECEIVE_SABME_CMD			26
#define DL_E_RECEIVE_UA_RSP			27
#define DL_E_RECEIVE_OTHER_CMD			28
#define DL_E_RECEIVE_OTHER_RSP			29
#define DL_E_RECEIVE_OTHER_CMD_OR_RSP		30
#define DL_E_RECEIVE_OTHER_CMD_INVALID_NR	31
#define DL_E_RECEIVE_OTHER_RSP_INVALID_NR	32
#define DL_E_P_TIMER_EXPIRED			33
#define DL_E_ACK_TIMER_EXPIRED			34
#define DL_E_REJ_TIMER_EXPIRED			35
#define DL_E_BUSY_TIMER_EXPIRED			36

/* Connection component action descriptions: (from IEEE 802.2 1998) */
#define DL_A_CLEAR_REMOTE_BUSY				 1
#define DL_A_CONNECT_INDICATION				 2
#define DL_A_CONNECT_CONFIRM				 3
#define DL_A_DATA_INDICATION				 4
#define DL_A_DISCONNECT_INDICATION			 5
#define DL_A_RESET_INDICATION				 6
#define DL_A_RESET_CONFIRM				 7
#define DL_A_REPORT_STATUS				 8
#define DL_A_IF_F_IS_1_CLEAR_REMOTE_BUSY		 9
#define DL_A_IF_DATA_FLAG_IS_2_STOP_REJ_TIMER		10
#define DL_A_SEND_DISC_CMD				11
#define DL_A_SEND_DM_RSP				12
#define DL_A_SEND_FRMR_RSP				13
#define DL_A_RESEND_FRMR_RSP_F_SET_TO_0			14
#define DL_A_RESEND_FRMR_RSP_F_SET_TO_P			15
#define DL_A_SEND_I_CMD_P_SET_TO_1			16
#define DL_A_RESEND_I_CMD_P_SET_TO_1			17
#define DL_A_RESEND_I_CMD_P_SET_TO_1_OR_SEND_RR		18
#define DL_A_SEND_I_OTHER				19
#define DL_A_RESEND_I_OTHER				20
#define DL_A_RESEND_I_OTHER_OR_SEND_RR			21
#define DL_A_RESEND_I_RSP_F_SET_TO_1			22
#define DL_A_SEND_REJ_CMD_P_BIT_SET			23
#define DL_A_SEND_REJ_RSP_F_BIT_SET			24
#define DL_A_SEND_REJ_CMD_OR_RSP_PF_CLEAR		25
#define DL_A_SEND_RNR_CMD_P_BIT_SET			26
#define DL_A_SEND_RNR_RSP_F_BIT_SET			27
#define DL_A_SEND_RNR_CMD_OR_RSP_PF_CLEAR		28
#define DL_A_SET_REMOTE_BUSY				29
#define DL_A_OPTIONAL_SEND_RNR_CMD_OR_RSP_PF_CLEAR	30
#define DL_A_SEND_RR_CMD_P_SET				31
#define DL_A_SEND_RR_RSP_F_SET				33
#define DL_A_SEND_RR_CMD_OR_RSP_PF_CLEAR		35
#define DL_A_SEND_ACKNOWLEDGE_CMD_P_SET			32
#define DL_A_SEND_ACKNOWLEDGE_RSP_F_SET			34
#define DL_A_SEND_ACKNOWLEDGE_CMD_OR_RSP_PF_CLEAR	36
#define DL_A_SEND_SABME_CMD				37
#define DL_A_SEND_UA_RSP				38
#define DL_A_CLR_S_FLAG					39
#define DL_A_SET_S_FLAG					40
#define DL_A_START_P_TIMER				41
#define DL_A_START_ACK_TIMER				42
#define DL_A_START_REJ_TIMER				43
#define DL_A_START_ACK_TIMER_IF_NOT_RUNNING		44
#define DL_A_STOP_ACK_TIMER				45
#define DL_A_STOP_P_TIMER				46
#define DL_A_STOP_REJ_TIMER				47
#define DL_A_STOP_ALL_TIMERS				48
#define DL_A_STOP_OTHER_TIMERS				49
#define DL_A_UPDATE_NR_RECEIVED				50
#define DL_A_UPDATE_P_FLAG				51
#define DL_A_DATA_FLAG_SET_TO_2				52
#define DL_A_DATA_FLAG_SET_TO_0				53
#define DL_A_DATA_FLAG_SET_TO_1				54
#define DL_A_IF_DATA_FLAG_CLEAR_THEN_SET_DATA_FLAG	55
#define DL_A_CLEAR_P_FLAG				56
#define DL_A_SET_P_FLAG					57
#define DL_A_SET_P_FLAG_TO_P				58
#define DL_A_CLEAR_REMOTE_BUSY				49
#define DL_A_CLEAR_RETRY_COUNT				50
#define DL_A_INCREMENT_RETRY_COUNT			51
#define DL_A_CLEAR_VR					52
#define DL_A_CLEAR_VS					53
#define DL_A_INCREMENET_VR				54
#define DL_A_SET_VS_TO_NR				55
#define DL_A_SET_F_FLAG_TO_P				56

/*
 * --------------------------------------------------------------------------
 *
 * HDLC AND LAPB RECEIVED MESSAGES
 *
 * --------------------------------------------------------------------------
 */
static fastcall noinline __unlikely int
dl_local_busy_detected(struct dl *dl, queue_t *q, mblk_t *mp)
{
	int err;

	switch (dl_get_pstate(dl)) {
	case DL_ST_NORMAL:
		if (dl->p_flag == 0) {
			if ((err = cd_snd_RNR(dl, q, mp, 1, 1, dl->vr)))
				return (err);
			mi_timer_stop(dl->t.p);
			dl->data_flag = 0;
			dl_set_pstate(dl, DL_ST_BUSY);
		} else {
			if ((err = cd_snd_RNR(dl, q, mp, 0, 0, dl->vr)))
				reutrn(err);
			dl->data_flag = 0;
			dl_set_pstate(dl, DL_ST_BUSY);
		}
		break;
	default:
		freemsg(mp);
		break;
	}
	return (0);
}

/**
 * dl_update_nr_received: - update received N(R)
 * @dl: data link private structure
 * @nr: the N(R) value to update
 *
 * UPDATE_N(R)_RECEIVED: If the N(R) of the received PDU acknowledges the receipt of one or more
 * previously unacknowledged I PDUs, update the local record of N(R)_RECEIVED, set RETRY_COUNT to
 * zero, and stop the acknowledgement timer.  If unacknowledged I PDUs still exist, start the
 * acknowledgement timer if it was stopped.
 *
 * NOTE -- If some form of SEND_I_PDU is initiated at the same time as UPDATE_N(R)_RECEIVED, then
 * the acknowledgement timer is always started if it was stopped.
 *
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |    tb     |     rtb     |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+
 *            |             |
 *            |             +--- N(R) - next sequence number unacknowleged
 *            +------------------V(S) - next sequence number to transmit
 *
 * Note: a recieved N(R) is invalid if it is not between N(R) and V(S)-1 inclusive.
 */
static fastcall inline void
dl_update_nr_received(struct dl *dl, int nr)
{
	if ((dl->vs > dl->nr) && (dl->vs > nr && nr > dl->nr) ||
	    (dl->vs < dl->nr) && (dl->vs > nr || nr > dl->nr)) {
		/* actually acknowledges one or more frames = */
		while ((dl->vs > dl->nr) && (dl->vs > nr && nr > dl->nr) ||
		       (dl->vs < dl->nr) && (dl->vs > nr || nr > dl->nr)) {
			mp = bufq_dequeue(&dl->rtb);
			freemsg(mp);
			dl->nr = (dl->nr + 1) & dl->mask;
		}
		dl->retry_count = 0;
		mi_timer_stop(dl->t.ack);
	}
}

static fastcall inline bool
dl_invalid_nr(struct dl *dl, int nr)
{
	if (nr == dl->nr) {
		/* acknowledges same frame as last received N(R) */
		return false;
	}
	if (dl->vs > dl->nr) {
		/* no wrap in number */
		if (dl->vs > nr && nr > dl->nr) {
			/* actually acknowledges one or more frames */
			return false;
		}
		/* completely wrong */
		return true;
	}
	if (dl->vs < dl->nr) {
		/* wrap in number */
		if (dl->vs > nr || nr > dl->nr) {
			/* actually acknowledges one or more frames */
			return false;
		}
		/* completely wrong */
		return true;
	}
	/* rtb is empty, nothing to acknowledge */
	return true;
}

/**
 * dl_rcv_I: - receive I (Information) frame
 * @dl: data link private structure
 * @q: active queue
 * @msg: message block to free upon success (or NULL)
 * @mp: the frame, split into A+C and I fields
 * @pf: P or F bit
 * @ns: N(S) sequence number
 * @nr: N(R) sequence number
 *
 * I - Information.  The function of the information (I) command is to transfer across a data link a
 * sequentially numbered frame containing an information field.
 */
static fastcall noinline __unlikely int
dl_rcv_I(struct dl *dl, queue_t *q, mblk_t *mp, int pf, int ns, int nr)
{
	int err;
	static const int cmd = 1;	/* only receive command frames for lapb */

	switch (dl_get_pstate(dl)) {
	case DL_ST_SETUP:
		break;
	case DL_ST_ADM:
		if (pf == 0)
			break;
		if ((err = cd_snd_DM(dl->cd, q, mp, pf)))
			goto error;
		return (0);
	case DL_ST_CONN:
		break;
	case DL_ST_RESET_WAIT:
		break;
	case DL_ST_RESET_CHECK:
		break;
	case DL_ST_RESET:
		break;
	case DL_ST_D_CONN:
		break;
	case DL_ST_ERROR:
		if ((err = cd_snd_FRMR(dl->cd, q, mp, pf)))
			goto error;
		mi_timer(dl->t.ack, dl->t1);
		dl_set_pstate(dl, DL_ST_ERROR);
		return (0);
	case DL_ST_NORMAL:
	case DL_ST_BUSY:
	case DL_ST_REJECT:
	case DL_ST_AWAIT:
	case DL_ST_AWAIT_BUSY:
	case DL_ST_AWAIT_REJECT:
		if (dl_invalid_ns(dl, ns) || dl_invalid_nr(dl, nr)) {
			if (cmd) {
				if ((err = cd_snd_FRMR(dl->cd, q, mp, pf, xxx)))
					goto error;
			} else {
				if ((err = cd_snd_FRMR(dl->cd, q, mp, 0, xxx)))
					goto error;
			}
			/* REPORT_STATUS(FRMR_SENT) */
			mi_timer_stop(dl->t.ack);
			mi_timer_stop(dl->t.p);
			mi_timer_stop(dl->t.rej);
			mi_timer_stop(dl->t.busy);
			dl->retry_count = 0;
			mi_timer(dl->t.ack, dl->t1);
			dl_set_pstate(dl, DL_ST_ERROR);
			return (0);
		}
		switch (dl_get_pstate(dl)) {
		case DL_ST_NORMAL:
			if (dl->vs != ns) {
				/* unexpected N(S) */
				if ((pf == 0 && dl->p_flag == 0) ||
				    (cmd == 0 && pf == 1 && dl->p_flag == 1)) {
					if ((err = cd_snd_REJ(dl->cd, q, mp, 1, 1, ns)))
						goto error;
					mi_timer(dl->t.p, dl->t1);
					if (pf == 1) {
						dl->remote_busy = 0;
						dl->txwakeup = 1;
					}
				} else if ((pf == 0 && dl->p_flag == 1)) {
					if ((err = cd_snd_REJ(dl->cd, q, mp, 1, 0, ns)))
						goto error;
				} else {
					if ((err = cd_snd_REJ(dl->cd, q, mp, 0, 1, ns)))
						goto error;
				}
				dl_update_nr_received(dl, nr);
				mi_timer(dl->t.rej, dl->t1);
				dl_set_state(dl, DL_ST_REJECT);
				return (0);
			}
			if ((cmd == 0 && dl->p_flag == pf) || (cmd == 1 && dl->p_flag == 0)) {
				if ((err = dl_data_ind(dl, q, mp, mp->b_cont)))
					goto error;
				dl->vr = (dl->vr + 1) & dl->mask;
				dl->sendack = 1;
				dl->txwakeup = 1;
				dl_update_nr_received(dl, nr);
				if (pf == 1) {
					dl->remote_busy = 0;
					dl->txwakeup = 1;
				}
			}
			if (pf == 0 && dl->p_flag == 1) {
			}
			if (cmd == 1 && pf == 1) {
			}
		case DL_ST_BUSY:
		case DL_ST_REJECT:
		case DL_ST_AWAIT:
		case DL_ST_AWAIT_BUSY:
		case DL_ST_AWAIT_REJECT:
		}

		if (ns != dl->vs) {
			cmd = (pf == 0) ? 1 : 0;
			pf = (pf == 0) ? (dl->p_flag ^ 0x01) : 1;
			if ((err = cd_snd_REJ(dl->cd, q, mp, cmd, pf, dl->vr)))
				goto error;
			if (cmd && pf)
				mi_timer(dl->t.p, dl->t1);
			dl_update_nr_received(dl, nr);
			mi_timer(dl->t.rej, dl->t1);
			dl_set_pstate(dl, DL_ST_REJECT);
			return (0);
		} else {
			if (pf == 0 && dl->p_flag == 0) {
				if ((err = dl_data_ind(dl, q, mp, mp->b_cont)))
					goto error;
			} else if (pf == 0 && dl->p_flag == 1) {
			} else {
			}
			dl->vr = (dl->vr + 1) & dl->mask;
		}
	}
	freemsg(mp);
	return (0);
      error:
	return (err);
}

/**
 * dl_rcv_RR: received an RR (Receive Ready) command or response
 * @dl: data link private structure
 * @q: active queue
 * @mp: M_PROTO message block of CD-primitive containing the frame
 * @cmd: true for command, false for response
 * @pf: P bit for command, F bit for response
 * @nr: N(R) value from supervisory frame
 *
 * RR - Receive Ready command, response.  The Receive Ready (RR) supervisory frame is used by the
 * DCE or DTE to: 1) indicate it is read to receive an I frame; and 2) acknowledge previosuly
 * received I frames numbered up to an including N(R)-1.  An RR frame may be used to indicate the
 * clearance of a busy condition that was reported by the earlier transmission of an RNR frame by
 * that same station (DCE or DTE).  In addition to indicate the DCE or DTE status, the RR command
 * with the P bit set to 1 may be used by the DCE or DTE to ask for the state of the DTE or DCE
 * respectively (that is, while Receive is Ready).
 */
static fastcall noinline __unlikely int
dl_rcv_RR(struct dl *dl, queue_t *q, mblk_t *mp, int cmd, int pf, int nr)
{
	int err;

	switch (dl_get_pstate(dl)) {
	case DL_ST_SETUP:
		break;
	case DL_ST_ADM:
		if (pf == 0 || cmd == 0)
			break;
		if ((err = cd_snd_DM(dl->cd, q, mp, pf)))
			return (err);
		return (0);
	case DL_ST_CONN:
		break;
	case DL_ST_RESET_WAIT:
		break;
	case DL_ST_RESET_CHECK:
		break;
	case DL_ST_RESET:
		break;
	case DL_ST_D_CONN:
		break;
	case DL_ST_ERROR:
		if (cmd == 0)
			break;
		if ((err = cd_snd_FRMR(dl->cd, q, mp, pf)))
			return (err);
		mi_timer(dl->t.ack, dl->t1);
		dl_set_pstate(dl, DL_ST_ERROR);
		return (0);
	}
	freemsg(mp);
	return (0);
}

/**
 * dl_rcv_RNR: received an RNR (Receive Not Ready) command or response
 * @dl: data link private structure
 * @q: active queue
 * @mp: M_PROTO message block of CD-primitive containing the frame
 * @cmd: true for command, false for response
 * @pf: P bit for command, F bit for response
 * @nr: N(R) value from supervisory frame
 *
 * RNR - Receive Not Ready command, response.  The Receive Not Ready (RNR) supervisory frame is used
 * by the DCE or DTE to indicate a busy condition; i.e., temporary inability to accept additional
 * incoming I frames.  I frames numbered up to and including N(R)-1 are acknowledged.  I frame N(R)
 * and any subsequent I frames received, if any, are not acknowledged; the acceptance status of
 * these I frames will be indicated in subsequent exchanges.  In addition to indicating the DCE or
 * DTE status, the RNR command with the P bit set to 1 may be used by a DCE or DTE to ask for the
 * status of the DTE or DCE respectively (that is, while Receive Not Ready).
 */
static fastcall noinline __unlikely int
dl_rcv_RNR(struct dl *dl, queue_t *q, mblk_t *mp, int cmd, int pf, int nr)
{
	int err;

	switch (dl_get_pstate(dl)) {
	case DL_ST_SETUP:
		break;
	case DL_ST_ADM:
		if (pf == 0 || cmd == 0)
			break;
		if ((err = cd_snd_DM(dl->cd, q, mp, pf)))
			return (err);
		return (0);
	case DL_ST_CONN:
		break;
	case DL_ST_RESET_WAIT:
		break;
	case DL_ST_RESET_CHECK:
		break;
	case DL_ST_RESET:
		break;
	case DL_ST_D_CONN:
		break;
	case DL_ST_ERROR:
		if (cmd == 0)
			break;
		if ((err = cd_snd_FRMR(dl->cd, q, mp, pf)))
			return (err);
		mi_timer(dl->t.ack, dl->t1);
		dl_set_pstate(dl, DL_ST_ERROR);
		return (0);
	}
	freemsg(mp);
	return (0);
}

/**
 * dl_rcv_REJ: received an REJ (Reject) command or response
 * @dl: data link private structure
 * @q: active queue
 * @mp: M_PROTO message block of CD-primitive containing the frame
 * @cmd: true for command, false for response
 * @pf: P bit for command, F bit for response
 * @nr: N(R) value from supervisory frame
 *
 * REJ - Reject command, response.  The reject (REJ) supervisory frame is used by the DCE or DTE to
 * request transmission of I frames starting with the frame numbered N(R).  I frames numbered N(R)-1
 * and below are acknowledged.  Additional I frames pending initial transmission may be transmitted
 * following the retransmitted I frame(s).  Only one REJ exception condition for a given direction
 * of information transfer may be established at any time.  THe REJ exception condition is cleared
 * (reset) upon the reciept of an I frame with an N(S) equal to the N(R) of the REJ frame.  An REJ
 * frame may be used to indicate the clearance of a busy condition that was reported by the earlier
 * transmission of an RNR frame by that same station (DCE or DTE).  In addition to indicating the
 * DCE or DTE status, the REJ command with the P bit set to 1 may be used by the DCE or DTE to ask
 * for the status of the DTE or DCE respectively.
 */
static fastcall noinline __unlikely int
dl_rcv_REJ(struct dl *dl, queue_t *q, mblk_t *mp, int cmd, int pf, int nr)
{
	int err;

	switch (dl_get_pstate(dl)) {
	case DL_ST_SETUP:
		break;
	case DL_ST_ADM:
		if (pf == 0 || cmd == 0)
			break;
		if ((err = cd_snd_DM(dl->cd, q, mp, pf)))
			return (err);
		return (0);
	case DL_ST_CONN:
		break;
	case DL_ST_RESET_WAIT:
		break;
	case DL_ST_RESET_CHECK:
		break;
	case DL_ST_RESET:
		break;
	case DL_ST_D_CONN:
		break;
	case DL_ST_ERROR:
		if (cmd == 0)
			break;
		if ((err = cd_snd_FRMR(dl->cd, q, mp, pf)))
			return (err);
		mi_timer(dl->t.ack, dl->t1);
		dl_set_pstate(dl, DL_ST_ERROR);
		return (0);
	}
	freemsg(mp);
	return (0);
}

/**
 * dl_rcv_SREJ: received an SREJ (Selective Reject) command or response
 * @dl: data link private structure
 * @q: active queue
 * @mp: M_PROTO message block of CD-primitive containing the frame
 * @cmd: true for command, false for response
 * @pf: P bit for command, F bit for response
 * @nr: N(R) value from supervisory frame
 *
 * SREJ - Selective Reject.  The selective reject (SREJ) supervisory frame shall be used by a DCE or
 * DTE to request retransmission of one ore more (not necessarily contiguous) I frames.  The N(R)
 * field of the control field of the SREJ frame shall contain the sequence number of the earliest I
 * frame to be retransmitted and the information field shall contain, in ascending order (i.e. 127
 * is higher than 126 and 0 is higher than 127 for modulo 128, and 32767 is higher than 32766 and 0
 * is higher than 32767 for modulo 32768), the sequence numbers of additional I frame(s), if any, in
 * need of retransmission.
 *
 * For extended (modulo 128) operation, the information field shall be encoded such that there is an
 * octet for each standalong I frame in need of retransmission, and a 2-octet span list for each
 * sequence of two or more contiguously number I frames in need of retransmission.  In the case of
 * standalong I frames, their identity in the information field consists of the appropriate N(R)
 * value preceded by a 0 bit in the octet used,.  In the case of span lists, their identity in the
 * information field constis of the N(R) value fo the first I frame in the span list preceived by a
 * 1 bit in the octet used, followed by the N(R) value of the last I frame in the span list preceded
 * by a 1 bit in the octet used.  The number of bits in an SREJ frame shall not exceed the value of
 * parameter N1, the maximum number of bits in an I frame.
 *
 * For super (modulo 32768) operation, the information field shall be encoded such that there is a
 * 2-octet field for each standalone I frame in need of restransmission, and a 4-octet span list for
 * each sequence of two or more contiguously numbered I frames in need of retransmission, as
 * depicted in table 2-11.  In the case of standalone I frames, their identity in the information
 * field consists of the N(R) value fo the first I frame in the span list preceded by a 1 bit in the
 * 2-octet field used, followed by the N(R) value of the last I frame in the span preceded by a 1
 * bit in the 2-octet field used.  The number of bits in the SREJ frame shall not exceed the value
 * of parameter N1, the maximum number of bits in an I frame.
 *
 * If the P/F bit in an SREJ is set to 1, then I frames number up to N(R)-1 includsive [N(R) being
 * the value in the control field] are considered acknowledged.  If the P/F bit in an SREJ frame is
 * set to 0, then the N(R) in the control field of the SREJ frame does not indicate acknkowledgement
 * of I frames.
 */
static fastcall noinline __unlikely int
dl_rcv_SREJ(struct dl *dl, queue_t *q, mblk_t *mp, int pf, int nr)
{
	int err;

	switch (dl_get_pstate(dl)) {
	case DL_ST_SETUP:
		break;
	case DL_ST_ADM:
		break;
	case DL_ST_CONN:
		break;
	case DL_ST_RESET_WAIT:
		break;
	case DL_ST_RESET_CHECK:
		break;
	case DL_ST_RESET:
		break;
	case DL_ST_D_CONN:
		break;
	case DL_ST_ERROR:
		break;
	}
	freemsg(mp);
	return (0);
}

/* UI - Unnumbered Information */
static fastcall noinline __unlikely int
dl_rcv_UI(struct dl *dl, queue_t *q, mblk_t *mp, int pf)
{
}

/* SABM - Set Asynchronous Balanced Mode */
static fastcall noinline __unlikely int
dl_rcv_SABM(struct dl *dl, queue_t *q, mblk_t *mp, int pf)
{
	int err;

	switch (dl_get_pstate(dl)) {
	case DL_ST_SETUP:
		if ((err = cd_snd_UA(dl, q, mp, pf)))
			return (err);
		dl->vs = 0;
		dl->vr = 0;
		dl->retry_count = 0;
		dl->s_flag = 1;
		// dl_set_pstate(dl, DL_ST_SETUP);
		return (0);
	case DL_ST_ADM:
		if ((err = dl_connect_ind(dl, q, mp)))
			return (err);
		dl->p_flag = pf;
		dl_set_pstate(dl, DL_ST_CONN);
		return (0);
	case DL_ST_CONN:
		dl->f_flag = pf;
		dl_set_pstate(dl, DL_ST_CONN);
		freemsg(mp);
		return (0);
	case DL_ST_RESET_WAIT:
		dl->s_flag = 1;
		dl->f_flag = pf;
		dl_set_pstate(dl, DL_ST_RESET_WAIT);
		freemsg(mp);
		return (0);
	case DL_ST_RESET_CHECK:
		dl->f_flag = pf;
		dl_set_pstate(dl, DL_ST_RESET_CHECK);
		freemsg(mp);
		return (0);
	case DL_ST_RESET:
		if ((err = cd_snd_UA(dl->cd, q, mp, pf)))
			return (err);
		dl->vs = 0;
		dl->vr = 0;
		dl->retry_count = 0;
		dl->s_flag = 1;
		dl_set_pstate(dl, DL_ST_RESET);
		return (0);
	case DL_ST_D_CONN:
		if ((err = cd_snd_DM(dl->cd, q, mp, pf)))
			return (err);
		mi_timer_stop(dl->t.ack);
		dl_set_pstate(dl, DL_ST_ADM);
		return (0);
	case DL_ST_ERROR:
		if ((err = dl_reset_ind(dl, q, mp)))
			return (err);
		mi_timer_stop(dl->t.ack);
		dl->f_flag = pf;
		dl_set_pstate(dl, DL_ST_RESET_CHECK);
		return (0);
	case DL_ST_NORMAL:
	case DL_ST_BUSY:
	case DL_ST_REJECT:
	case DL_ST_AWAIT:
	case DL_ST_AWAIT_BUSY:
	case DL_ST_AWAIT_REJECT:
		if ((err = dl_reset_ind(dl, q, mp)))
			return (err);
		dl->f_flag = pf;
		mi_timer_stop(dl->t.ack);
		mi_timer_stop(dl->t.p);
		mi_timer_stop(dl->t.rej);
		mi_timer_stop(dl->t.busy);
		dl_set_pstate(dl, DL_ST_ADM);
		return (0);
	}
}

/* SABME - Set Asynchronous Balanced Mode Extended */
static fastcall noinline __unlikely int
dl_rcv_SABME(struct dl *dl, queue_t *q, mblk_t *mp, int pf)
{
	int err;

	switch (dl_get_pstate(dl)) {
	case DL_ST_SETUP:
		if ((err = cd_snd_UA(dl, q, mp, pf)))
			return (err);
		dl->vs = 0;
		dl->vr = 0;
		dl->retry_count = 0;
		dl->s_flag = 1;
		// dl_set_pstate(dl, DL_ST_SETUP);
		return (0);
	case DL_ST_ADM:
		if ((err = dl_connect_ind(dl, q, mp)))
			return (err);
		dl->p_flag = pf;
		dl_set_pstate(dl, DL_ST_CONN);
		return (0);
	case DL_ST_CONN:
		dl->f_flag = pf;
		dl_set_pstate(dl, DL_ST_CONN);
		freemsg(mp);
		return (0);
	case DL_ST_RESET_WAIT:
		dl->s_flag = 1;
		dl->f_flag = pf;
		dl_set_pstate(dl, DL_ST_RESET_WAIT);
		freemsg(mp);
		return (0);
	case DL_ST_RESET_CHECK:
		dl->f_flag = pf;
		dl_set_pstate(dl, DL_ST_RESET_CHECK);
		freemsg(mp);
		return (0);
	case DL_ST_RESET:
		if ((err = cd_snd_UA(dl->cd, q, mp, pf)))
			return (err);
		dl->vs = 0;
		dl->vr = 0;
		dl->retry_count = 0;
		dl->s_flag = 1;
		dl_set_pstate(dl, DL_ST_RESET);
		return (0);
	case DL_ST_D_CONN:
		if ((err = cd_snd_DM(dl->cd, q, mp, pf)))
			return (err);
		mi_timer_stop(dl->t.ack);
		dl_set_pstate(dl, DL_ST_ADM);
		return (0);
	case DL_ST_ERROR:
		if ((err = dl_reset_ind(dl, q, mp)))
			return (err);
		mi_timer_stop(dl->t.ack);
		dl->f_flag = pf;
		dl_set_pstate(dl, DL_ST_RESET_CHECK);
		return (0);
	case DL_ST_NORMAL:
	case DL_ST_BUSY:
	case DL_ST_REJECT:
	case DL_ST_AWAIT:
	case DL_ST_AWAIT_BUSY:
	case DL_ST_AWAIT_REJECT:
		if ((err = dl_reset_ind(dl, q, mp)))
			return (err);
		dl->f_flag = pf;
		mi_timer_stop(dl->t.ack);
		mi_timer_stop(dl->t.p);
		mi_timer_stop(dl->t.rej);
		mi_timer_stop(dl->t.busy);
		dl_set_pstate(dl, DL_ST_ADM);
		return (0);
	}
}

/* SM - Set Mode */
static fastcall noinline __unlikely int
dl_rcv_SM(struct dl *dl, queue_t *q, mblk_t *mp, int pf)
{
	int err;

	switch (dl_get_pstate(dl)) {
	case DL_ST_SETUP:
		if ((err = cd_snd_UA(dl, q, mp, pf)))
			return (err);
		dl->vs = 0;
		dl->vr = 0;
		dl->retry_count = 0;
		dl->s_flag = 1;
		// dl_set_pstate(dl, DL_ST_SETUP);
		return (0);
	case DL_ST_ADM:
		if ((err = dl_connect_ind(dl, q, mp)))
			return (err);
		dl->p_flag = pf;
		dl_set_pstate(dl, DL_ST_CONN);
		return (0);
	case DL_ST_CONN:
		dl->f_flag = pf;
		dl_set_pstate(dl, DL_ST_CONN);
		freemsg(mp);
		return (0);
	case DL_ST_RESET_WAIT:
		dl->s_flag = 1;
		dl->f_flag = pf;
		dl_set_pstate(dl, DL_ST_RESET_WAIT);
		freemsg(mp);
		return (0);
	case DL_ST_RESET_CHECK:
		dl->f_flag = pf;
		dl_set_pstate(dl, DL_ST_RESET_CHECK);
		freemsg(mp);
		return (0);
	case DL_ST_RESET:
		if ((err = cd_snd_UA(dl->cd, q, mp, pf)))
			return (err);
		dl->vs = 0;
		dl->vr = 0;
		dl->retry_count = 0;
		dl->s_flag = 1;
		dl_set_pstate(dl, DL_ST_RESET);
		return (0);
	case DL_ST_D_CONN:
		if ((err = cd_snd_DM(dl->cd, q, mp, pf)))
			return (err);
		mi_timer_stop(dl->t.ack);
		dl_set_pstate(dl, DL_ST_ADM);
		return (0);
	case DL_ST_ERROR:
		if ((err = dl_reset_ind(dl, q, mp)))
			return (err);
		mi_timer_stop(dl->t.ack);
		dl->f_flag = pf;
		dl_set_pstate(dl, DL_ST_RESET_CHECK);
		return (0);
	case DL_ST_NORMAL:
	case DL_ST_BUSY:
	case DL_ST_REJECT:
	case DL_ST_AWAIT:
	case DL_ST_AWAIT_BUSY:
	case DL_ST_AWAIT_REJECT:
		if ((err = dl_reset_ind(dl, q, mp)))
			return (err);
		dl->f_flag = pf;
		mi_timer_stop(dl->t.ack);
		mi_timer_stop(dl->t.p);
		mi_timer_stop(dl->t.rej);
		mi_timer_stop(dl->t.busy);
		dl_set_pstate(dl, DL_ST_ADM);
		return (0);
	}
}

/* DISC - Disconnect */
static fastcall noinline __unlikely int
dl_rcv_DISC(struct dl *dl, queue_t *q, mblk_t *mp, int cmd, int pf)
{
	int err;

	switch (dl_get_pstate(dl)) {
	case DL_ST_SETUP:
		if (dl_get_state(dl) != DL_IDLE)
			if ((err = dl_disconnect_ind(dl, q, NULL)))
				return (err);
		if ((err = cd_snd_DM(dl->cd, q, mp, pf)))
			return (err);
		mi_timer_stop(dl->t.ack);
		dl_set_pstate(dl, DL_ST_ADM);
		return (0);
	case DL_ST_ADM:
		if ((err = cd_snd_DM(dl->cd, q, mp, pf)))
			return (err);
		dl_set_pstate(dl, DL_ST_ADM);
		return (0);
	case DL_ST_CONN:
		break;
	case DL_ST_RESET_WAIT:
		if (dl_get_state(dl) != DL_IDLE)
			if ((err = dl_disconnect_ind(dl, q, NULL)))
				return (err);
		if ((err = cd_snd_DM(dl->cd, q, mp, pf)))
			return (err);
		dl_set_pstate(dl, DL_ST_ADM);
		return (0);
	case DL_ST_RESET_CHECK:
		if (dl_get_state(dl) != DL_IDLE)
			if ((err = dl_disconnect_ind(dl, q, NULL)))
				return (err);
		if ((err = cd_snd_DM(dl->cd, q, mp, pf)))
			return (err);
		dl_set_pstate(dl, DL_ST_ADM);
		return (0);
	case DL_ST_RESET:
		if (dl_get_state(dl) != DL_IDLE)
			if ((err = dl_disconnect_ind(dl, q, NULL)))
				return (err);
		if ((err = cd_snd_DM(dl->cd, q, mp, pf)))
			return (err);
		mi_timer_stop(dl->t.ack);
		dl_set_pstate(dl, DL_ST_ADM);
		return (0);
	case DL_ST_D_CONN:
		break;
	case DL_ST_ERROR:
		if (dl_get_state(dl) != DL_IDLE)
			if ((err = dl_disconnect_ind(dl, q, NULL)))
				return (err);
		if ((err = cd_snd_DM(dl->cd, q, mp, pf)))
			return (err);
		mi_timer_stop(dl->t.ack);
		dl_set_pstate(dl, DL_ST_ADM);
		return (0);
	case DL_ST_NORMAL:
	case DL_ST_BUSY:
	case DL_ST_REJECT:
	case DL_ST_AWAIT:
	case DL_ST_AWAIT_BUSY:
	case DL_ST_AWAIT_REJECT:
		if (dl_get_state(dl) != DL_IDLE)
			if ((err = dl_disconnect_ind(dl, q, NULL)))
				return (err);
		if ((err = cd_snd_UA(dl->cd, q, mp, pf)))
			return (err);
		dl - timer_stop(dl, DL_TIMER_ALL);
		dl_set_pstate(dl, DL_ST_ADM);
		return (0);
	}
	freemsg(mp);
	return (0);
}

/* DM - Disconnect Mode */
static fastcall noinline __unlikely int
dl_rcv_DM(struct dl *dl, queue_t *q, mblk_t *mp, int pf)
{
	int err;

	switch (dl_get_pstate(dl)) {
	case DL_ST_SETUP:
		if ((err = dl_disconnect_ind(dl, q, mp)))
			return (err);
		mi_timer_stop(dl->t.ack);
		dl_set_pstate(dl, DL_ST_ADM);
		return (0);
	case DL_ST_ADM:
		break;
	case DL_ST_CONN:
		if ((err = dl_disconnect_ind(dl, q, mp)))
			return (err);
		dl_set_pstate(dl, DL_ST_ADM);
		return (0);
	case DL_ST_RESET_WAIT:
		if ((err = dl_disconnect_ind(dl, q, mp)))
			return (err);
		dl_set_pstate(dl, DL_ST_ADM);
		return (0);
	case DL_ST_RESET_CHECK:
		if ((err = dl_disconnect_ind(dl, q, mp)))
			return (err);
		dl_set_pstate(dl, DL_ST_ADM);
		return (0);
	case DL_ST_RESET:
		if ((err = dl_disconnect_ind(dl, q, mp)))
			return (err);
		mi_timer_stop(dl->t.ack);
		dl_set_pstate(dl, DL_ST_ADM);
		return (0);
	case DL_ST_D_CONN:
		break;
	case DL_ST_ERROR:
		if ((err = dl_disconnect_ind(dl, q, mp)))
			return (err);
		mi_timer_stop(dl->t.ack);
		dl_set_pstate(dl, DL_ST_ADM);
		return (0);
	case DL_ST_NORMAL:
	case DL_ST_BUSY:
	case DL_ST_REJECT:
	case DL_ST_AWAIT:
	case DL_ST_AWAIT_BUSY:
	case DL_ST_AWAIT_REJECT:
		if ((err = dl_disconnect_ind(dl, q, mp)))
			return (err);
		mi_timer_stop(dl->t.ack);
		dl_set_pstate(dl, DL_ST_ADM);
		return (0);
	}
	freemsg(mp);
	return (0);
}

/* UA - Unnumbered Acknowledgement */
static fastcall noinline __unlikely int
dl_rcv_UA(struct dl *dl, queue_t *q, mblk_t *mp, int pf)
{
	int err;

	switch (dl_get_pstate(dl)) {
	case DL_ST_SETUP:
		if (pf != dl->ps)
			break;
		if ((err = dl_connect_con(dl, q, mp)))
			return (err);
		mi_timer_stop(dl->t.ack);
		dl->vs = 0;
		dl->vr = 0;
		dl->retry_count = 0;
		if (pf == 1) {
			dl->p_flag = 0;
			mi_timer_stop(dl->t.p);
		}
		dl->remote_busy = 0;
		dl_set_pstate(dl, DL_ST_NORMAL);
		return (0);
	case DL_ST_ADM:
		break;
	case DL_ST_CONN:
		break;
	case DL_ST_RESET_WAIT:
		break;
	case DL_ST_RESET_CHECK:
		break;
	case DL_ST_RESET:
		if (pf != dl->p_flag)
			break;
		if ((err = dl_reset_con(dl, q, mp)))
			return (err);
		mi_timer_stop(dl->t.ack);
		dl->vs = 0;
		dl->vr = 0;
		dl->retry_count = 0;
		if (pf == 1) {
			dl->p_flag = 0;
			mi_timer_stop(dl->t.p);
		}
		dl->remote_busy = 0;
		dl_set_pstate(dl, DL_ST_NORMAL);
		return (0);
	case DL_ST_D_CONN:
		if (dl->p_flag != pf)
			break;
		mi_timer_stop(dl->t.ack);
		dl_set_pstate(dl, DL_ST_ADM);
		freemsg(mp);
		return (0);
	case DL_ST_ERROR:
		break;
	case DL_ST_NORMAL:
	case DL_ST_BUSY:
	case DL_ST_REJECT:
	case DL_ST_AWAIT:
	case DL_ST_AWAIT_BUSY:
	case DL_ST_AWAIT_REJECT:
		if ((err = cd_snd_FRMR(dl->cd, q, mp)))
			return (err);
		/* REPORT_STATUS(FRMR_SENT) */
		mi_timer_stop(dl->t.ack);
		mi_timer_stop(dl->t.p);
		mi_timer_stop(dl->t.rej);
		mi_timer_stop(dl->t.busy);
		mi_timer(dl->t.ack, dl->t1);
		dl->retry_count = 0;
		dl_set_pstate(dl, DL_ST_ERROR);
		return (0);
	}
	freemsg(mp);
	return (0);
}

/* FRMR - Frame Reject */
static fastcall noinline __unlikely int
dl_rcv_FRMR(struct dl *dl, queue_t *q, mblk_t *mp, int pf)
{
	int err;

	switch (dl_get_pstate(dl)) {
	case DL_ST_SETUP:
		break;
	case DL_ST_ADM:
		break;
	case DL_ST_CONN:
		break;
	case DL_ST_RESET_WAIT:
		break;
	case DL_ST_RESET_CHECK:
		break;
	case DL_ST_RESET:
		break;
	case DL_ST_D_CONN:
		break;
	case DL_ST_ERROR:
		if ((err = dl_reset_ind(dl, q, mp)))
			return (err);
		mi_timer_stop(dl->t.ack);
		/* REPORT_STATUS(FRMR_RECEIVED) */
		dl->s_flag = 0;
		dl_set_pstate(dl, DL_ST_RESET_WAIT);
		return (0);
	case DL_ST_NORMAL:
	case DL_ST_BUSY:
	case DL_ST_REJECT:
	case DL_ST_AWAIT:
	case DL_ST_AWAIT_BUSY:
	case DL_ST_AWAIT_REJECT:
		if ((err = dl_reset_ind(dl, q, mp)))
			return (err);
		/* REPORT_STATUS(FRMR_RECEIVED) */
		mi_timer_stop(dl->t.ack);
		mi_timer_stop(dl->t.p);
		mi_timer_stop(dl->t.rej);
		mi_timer_stop(dl->t.busy);
		dl->s_flag = 0;
		dl_set_pstate(dl, DL_ST_RESET_WAIT);
		return (0);
	}
	freemsg(mp);
	return (0);
}

/* XID - Exchange Identification command, response */
static fastcall noinline __unlikely int
dl_rcv_XID(struct dl *dl, queue_t *q, mblk_t *mp, int cmd, int pf)
{
}

static fastcall inline __hot_read int
dl_rcv_msg(struct dl *dl, queue_t *q, mblk_t *msg, mblk_t *mp)
{
	int ns, pf, nr;
	int ext = dl_extended(dl);
	int sup = dl_supermode(dl);
	int mod = (sup << 1) | (ext ^ sup);
	int num = ((mp->b_rptr[1] & 0x03) != 0x03);
	int inf = ((mp->b_rptr[1] & 0x01) != 0x00);
	int size = msgdsize(mp);
	int cmd = 0;

	if (size < 2)
		goto discard;
	if (num && size < 2 + mod)
		goto badsize;
	switch (mod) {
	case 00:		/* basic -modulo 8 */
		nr = (mp->b_rptr[1] >> 5) & 0x07;
		ns = (mp->b_rptr[1] >> 1) & 0x07;
		pf = (mp->b_rptr[1] >> 4) & 0x01;
		break;
	case 01:		/* extended - modulo 127 */
		nr = (mp->b_rptr[2] >> 1) & 0x7f;
		ns = (mp->b_rptr[1] >> 1) & 0x7f;
		pf = (mp->b_rptr[2] >> 0) & 0x01;
		break;
	case 03:		/* super - modulo 32768 */
		nr = ((int) mp->b_rtpr[4] << 7) | ((int) mp->b_rptr[3] >> 1);
		nr = ((int) mp->b_rtpr[2] << 7) | ((int) mp->b_rptr[1] >> 1);
		pf = (mp->b_rptr[3] >> 0) & 0x01;
		break;
	case 02:		/* illegal */
		mi_strlog(q, STRLOGRX, SL_TRACE, "discarding illegal mode");
		goto discard;
	}
	/* Addresses: SLP: A 0x03 DCE->DTE command DCE<-DTE response B 0x01 DTE->DCE command
	   DTE<-DCE response MLP: C 0x0f DCE->DTE command DCE<-DTE response D 0x07 DTE->DCE command 
	   DTE<-DCE response */
	if ((mp->b_rptr[0] & 0xf0) != 0)
		goto discard;	/* bad address */
	if (mp->b_rptr[0] != dl->raddr && mp->b_rptr[0] != dl->caddr)
		goto discard;
	/* Address on which I receive command is that on which I send responses. */
	/* Address on which I receive responses is that on which I sen commands. */
	cmd = (mp->b_rptr[0] == dl->raddr) ? 1 : 0;

	switch (mp->b_rptr[1] & 0x03) {
	case 0x00:
	case 0x02:		/* Information *//* command */
		if (!cmd)
			goto discard;
		if (size > 2 + ext + dl->proto.info.dl_max_sdu)
			goto toolong;
		return dl_rcv_I(dl, q, msg, mp, pf, ns, nr);

	case 0x01:		/* Supervisory */
		switch ((mp->b_rptr[1] >> 2) & 0x03) {
		case 0x00:	/* RR *//* command, response */
			if (size != 2 + mod)
				goto badsize;
			return dl_rcv_RR(dl, q, msg, mp, cmd, pf, nr);
		case 0x01:	/* RNR *//* command, response */
			if (size != 2 + mod)
				goto badsize;
			return dl_rcv_RNR(dl, q, msg, mp, cmd, pf, nr);
		case 0x02:	/* REJ *//* command, response */
			if (size != 2 + mod)
				goto badsize;
			return dl_rcv_REJ(dl, q, msg, mp, cmd, pf, nr);
		case 0x03:	/* SREJ *//* response */
			if (cmd)
				goto discard;
			return dl_rcv_SREJ(dl, q, msg, mp, pf, nr);
		}
	case 0x03:		/* Unnumbered */
		if ((mp->b_rptr[1] & 0x37) != 0x21 && size != 2)
			goto badsize;
		switch ((mp->b_rptr[1] >> 2) & 0x3b) {
		case 0x00:	/* UI *//* command *//* 0000 0000 */
			if (!cmd)
				goto discard;
			return dl_rcv_UI(dl, q, msg, mp, pf);
		case 0x0b:	/* SABM *//* command *//* 0000 1011 */
			if (!cmd)
				goto discard;
			return dl_rcv_SABM(dl, q, msg, mp, pf);
		case 0x1b:	/* SABME *//* command *//* 0001 1011 */
			if (!cmd)
				goto discard;
			return dl_rcv_SABME(dl, q, msg, mp, pf);
		case 0x30:	/* SM *//* command *//* 0011 000 */
			if (!cmd)
				goto discard;
			return dl_rcv_SM(dl, q, msg, mp, pf);
		case 0x10:	/* DISC *//* command *//* 0001 0000 */
			if (!cmd)
				goto discard;
			return dl_rcv_DISC(dl, q, msg, mp, pf);
		case 0x03:	/* DM *//* response *//* 0000 0011 */
			if (cmd)
				goto discard;
			return dl_rcv_DM(dl, q, msg, mp, pf);
		case 0x18:	/* UA *//* response *//* 0001 1000 */
			if (cmd)
				goto discard;
			return dl_rcv_UA(dl, q, msg, mp, pf);
		case 0x21:	/* FRMR *//* response *//* 0010 0001 */
			if (cmd)
				goto discard;
			return dl_rcv_FRMR(dl, q, msg, mp, pf);
		case 0x2b:	/* XID *//* command, response *//* 0010 1011 */
			return dl_rcv_XID(dl, q, msg, mp, cmd, pf);
		default:
			goto unrecog;
		}
	}
	{
		int flags;

		/* Frame rejection condition: A frame rejection condition is established upon the
		   receipt of an error-free frame with one of the conditions listed below.  At the
		   DCE or DTE, this frame rejection exception condition is reported by an FRMR
		   response for approprite DTE or DCE action, respectively. Once a DCE has
		   established such an exception condition, no additional I frames are accepted
		   until the condition is reset by the DTE, except for examination of the P bit. The 
		   FRMR response may be repeated at each opportunity, until recovery is effected by
		   the DTE, or until the DCE initiates its own recovery in case the DTE does not
		   respond. */
	      badnr:
#define DL_REJ_FLAG_BADNR	0x10
		/* send an FRMR for bad N(R) */
		flags = DL_REJ_FLAG_BADNR;
		goto error;
	      toolong:
		/* send an FRMR for too long info frame */
#define DL_REJ_FLAG_TOOLONG	0x20
		flags = DL_REJ_FLAG_TOOLONG;
		goto error;
	      badsize:
		/* send an FRMR for bad size has info or wrong size */
#define DL_REJ_FLAG_BADSIZE	0xC0
		flags = DL_REJ_FLAG_BADSIZE;
		goto error;
	      unrecog:
		/* send an FRMR for unrecognized */
#define DL_REJ_FLAG_UNDEF	0x80
		flags = DL_REJ_FLAG_UNDEF;
		goto error;
	      error:
		/* send an FRMR */
		return cd_snd_FRMR(dl->cd, q, msg, mp, rfcf, pf, vs, cr, vr, flags);
	}
	/* Invalid frame condition: Any frame that is invalid will be discarded, and no action
	   taken as a result of that frame.  An invalid frame is defined as one which: a) is not
	   properly bounded by two flags; b) in basic (modulo 8) operation, contains fewer than 32
	   bits between flags; in extended (modulo 128) operation, contains fewer than 40 bits
	   between flags of frames that contain sequence numbers or 32 bits between flags of frames
	   that do not contain sequence numbers; c) or start/stop transmission, in addition to
	   conditions listed in b), contains an octet-framing violoation (i.e. a 0 bit occurs where
	   a stop bit is expected); d) contains a Frame Check Sequence (FCS) error; e) contains an
	   address other than A or B (for single link operation) or other than C or D (for multilink 
	   operation); or, f) frame aborted: in synchronous transmission, a frame is aborted when it 
	   contains at least seven contiguous 1 bits (with no inserted 0 bit); in start/stop
	   transmission, a frame is aborted when it contains the two-octet sequence composed fo the
	   control escape octet followed by a closing flag.  For those networks that are octet
	   aligned, a detection of non-octet alignment may be made at the Data Link Layer by adding
	   a frame validity check that requires the number of bits between the opening flag and the
	   closing flag, excluding inserted bits (for transparency or for transmission timing for
	   start/stop transmission), to be an integral number of octets in length, or the frame is
	   considered invalid. */
      discard:
	if (msg && msg->b_cont == mp)
		msg->b_cont = NULL;
	freemsg(msg);
	freemsg(mp);
	return (0);
}

/*
 * --------------------------------------------------------------------------
 *
 * LAPB TIMERS
 *
 * --------------------------------------------------------------------------
 */

static fastcall noinline __unlikely int
dl_timeout_ack(struct dl *dl, queue_t *q)
{
	int err;

	switch (dl_get_pstate(dl)) {
	case DL_ST_SETUP:
		if (dl->s_flag == 1) {
			if ((err = dl_connect_con(dl, q, NULL)))
				return (err);
			dl->p_flag = 0;
			dl->remote_busy = 0;
			dl_set_pstate(dl, DL_ST_NORMAL);
			return (0);
		} else if (dl->retry_count < dl->n2) {
			if ((err = cd_snd_SM(dl, q, NULL, 1)))
				return (err);
			dl->p_flag = 1;
			mi_timer(dl->t.ack, dl->t1);
			dl->retry_count++;
			// dl_set_pstate(dl, DL_ST_SETUP);
			return (0);
		} else {
			if ((err = dl_disconnect_ind(dl, q, NULL)))
				return (err);
			dl_set_pstate(dl, DL_ST_ADM);
			return (0);
		}
	case DL_ST_RESET:
		if (dl->s_flag == 1) {
			if ((err = dl_reset_con(dl, q, NULL)))
				return (err);
			dl->p_flag = 0;
			dl->remote_busy = 0;
			dl_set_pstate(dl, DL_ST_NORMAL);
			return (0);

		} else if (dl->retry_count < dl->n2) {
			if ((err = cd_snd_SM(dl, q, NULL, 1)))
				return (err);
			dl->p_flag = 1;
			mi_timer(dl->t.ack, dl->t1);
			dl->retry_count++;
			dl_set_pstate(dl, DL_ST_RESET);
			return (0);
		} else {
			if ((err = dl_disconnect_ind(dl, q, NULL)))
				return (err);
			dl_set_pstate(dl, DL_ST_ADM);
			return (0);
		}
	case DL_ST_D_CONN:
		if (dl->retry_count < dl->n2) {
			if ((err = cd_snd_DISC(dl, q, NULL, 1)))
				return (err);
			dl->p_flag = 1;
			mi_timer(dl->t.ack, dl->t1);
			dl->retry_count++;
			dl_set_pstate(dl, DL_ST_CONN);
			return (0);
		} else {
			dl_set_pstate(dl, DL_ST_ADM);
			return (0);
		}
	case DL_ST_ERROR:
		if (dl->retry_count < dl->n2) {
			if ((err = cd_snd_FRMR(dl, q, NULL, 0)))
				return (err);
			mi_timer(dl->t.ack, dl->t1);
			dl->retry_count++;
			dl_set_pstate(dl, DL_ST_ERROR);
			return (0);
		} else {
			if ((err = dl_reset_ind(dl, q, NULL)))
				return (err);
			dl->s_flag = 0;
			dl_set_pstate(dl, DL_ST_RESET_WAIT);
			return (0);
		}
	case DL_ST_NORMAL:
	case DL_ST_BUSY:
	case DL_ST_REJECT:
	case DL_ST_AWAIT:
	case DL_ST_AWAIT_BUSY:
	case DL_ST_AWAIT_REJECT:
		if (dl->retry_count >= dl->n2) {
			if ((err = dl_reset_ind(dl, q, NULL)))
				return (err);
			mi_timer_stop(dl->t.ack);
			mi_timer_stop(dl->t.p);
			mi_timer_stop(dl->t.rej);
			mi_timer_stop(dl->t.busy);
			dl->s_flag = 0;
			dl_set_pstate(dl, DL_ST_RESET_WAIT);
			return (0);
		}
		return (0);
	}
	return (0);
}

static fastcall noinline __unlikely int
dl_timeout_p(struct dl *dl, queue_t *q)
{
	int err;

	switch (dl_get_pstate(dl)) {
	case DL_ST_NORMAL:
	case DL_ST_BUSY:
	case DL_ST_REJECT:
	case DL_ST_AWAIT:
	case DL_ST_AWAIT_BUSY:
	case DL_ST_AWAIT_REJECT:
		if (dl->retry_count >= dl->n2) {
			if ((err = dl_reset_ind(dl, q, NULL)))
				return (err);
			mi_timer_stop(dl->t.ack);
			mi_timer_stop(dl->t.p);
			mi_timer_stop(dl->t.rej);
			mi_timer_stop(dl->t.busy);
			dl->s_flag = 0;
			dl_set_pstate(dl, DL_ST_RESET_WAIT);
			return (0);
		}
		return (0);
	}
	return (0);
}

static fastcall noinline __unlikely int
dl_timeout_rej(struct dl *dl, queue_t *q)
{
	int err;

	switch (dl_get_pstate(dl)) {
	case DL_ST_NORMAL:
	case DL_ST_BUSY:
	case DL_ST_REJECT:
	case DL_ST_AWAIT:
	case DL_ST_AWAIT_BUSY:
	case DL_ST_AWAIT_REJECT:
		if (dl->retry_count >= dl->n2) {
			if ((err = dl_reset_ind(dl, q, NULL)))
				return (err);
			mi_timer_stop(dl->t.ack);
			mi_timer_stop(dl->t.p);
			mi_timer_stop(dl->t.rej);
			mi_timer_stop(dl->t.busy);
			dl->s_flag = 0;
			dl_set_pstate(dl, DL_ST_RESET_WAIT);
			return (0);
		}
		return (0);
	}
	return (0);
}

static fastcall noinline __unlikely int
dl_timeout_busy(struct dl *dl, queue_t *q)
{
	int err;

	switch (dl_get_pstate(dl)) {
	case DL_ST_NORMAL:
	case DL_ST_BUSY:
	case DL_ST_REJECT:
	case DL_ST_AWAIT:
	case DL_ST_AWAIT_BUSY:
	case DL_ST_AWAIT_REJECT:
		if (dl->retry_count >= dl->n2) {
			if ((err = dl_reset_ind(dl, q, NULL)))
				return (err);
			mi_timer_stop(dl->t.ack);
			mi_timer_stop(dl->t.p);
			mi_timer_stop(dl->t.rej);
			mi_timer_stop(dl->t.busy);
			dl->s_flag = 0;
			dl_set_pstate(dl, DL_ST_RESET_WAIT);
			return (0);
		}
		return (0);
	}
	return (0);
}

/*
 * --------------------------------------------------------------------------
 *
 * DLPI USER TO DLPI PROVIDER RECEIVED PRIMITIVES
 *
 * --------------------------------------------------------------------------
 */
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
		goto tooshort;
	if (dl->proto.info.dl_provider_style != DL_STYLE2)
		goto notsupported;
	if (dl_get_state(dl) == DL_UNATTACHED)
		goto outstate;
	dl->ppa = p->dl_ppa;
	dl_set_state(dl, DL_ATTACH_PENDING);
	return cd_attach_req(dl->cd, q, mp, p->dl_ppa);
      notsuported:
	err = DL_NOTSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      tooshort:
	err = -EFAULT;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_ATTACH_REQ, err);
}
static fastcall noinline __unlikely int
dl_detach_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_detach_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (dl->proto.info.dl_provider_style != DL_STYLE2)
		goto notsupported;
	if (dl_get_state(dl) != DL_UNBOUND)
		goto outstate;
	dl_set_state(dl, DL_DETACH_PENDING);
	return cd_detach_req(dl->cd, q, mp);
      notsuported:
	err = DL_NOTSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      tooshort:
	err = -EFAULT;
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
		goto tooshort;
	if (dl_get_state(dl) != DL_UNBOUND)
		goto outstate;
	if ((dl->proto.info.dl_service_mode ^ p->dl_service_mode) & p->dl_service_mode)
		goto unsupported;
	if ((p->dl_xidtest_flg & (DL_AUTO_XID | DL_AUTO_TEST)) == (DL_AUTO_XID | DL_AUTO_TEST))
		goto noauto;
	if (p->dl_xidtest_flg & DL_AUTO_XID)
		goto noxidauto;
	if (p->dl_xidtest_flg & DL_AUTO_TEST)
		goto notestauto;
	if (p->dl_sap == 0)
		goto noaddr;
	dl->proto.info.dl_sap = p->dl_sap;
	dl_set_state(dl, DL_BIND_PENDING);
	/* First we need to enable the cd */
	if (cd_get_state(cd) != CD_ENABLED)
		return cd_enable_req(dl->cd, q, mp);
	return dl_bind_ack(dl, q, mp);
      noaddr:
	err = DL_NOADDR;
	break;
      notestauto:
	err = DL_NOTESTAUTO;
	break;
      noxidauto:
	err = DL_NOXIDAUTO;
	goto error;
      noauto:
	err = DL_NOAUTO;
	goto error;
      unsupported:
	err = DL_UNSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      tooshort:
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
	if (cd_get_state(dl->cd) != CD_DISABLED)
		return cd_disable_req(dl->cd, q, mp, CD_FLUSH);
	return dl_ok_ack(dl, q, mp, DL_UNBIND_REQ);
      outstate:
	err = DL_OUTSTATE;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_UNBIND_REQ, err);
}
static fastcall noinline __unlikely int
dl_subs_bind_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_subs_bind_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->dl_subs_sap_offset, p->dl_subs_sap_length))
		goto badaddr;
	switch (p->dl_subs_bind_class) {
	case DL_PEER_BIND:
		break;
	case DL_HIERARCHICAL_BIND:
		goto toomany;
	default:
		goto unsupported;
	}
	if (dl_get_state(dl) != DL_IDLE)
		goto outstate;
	dl_set_state(dl, DL_SUBS_BIND_PND);
	goto notsupported;
      toomany:
	err = DL_TOOMANY;
	goto error;
      notsuported:
	err = DL_NOTSUPPORTED;
	goto error;
      unsupported:
	err = DL_UNSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      badaddr:
	err = DL_BADADDR;
	goto error;
      badqos:
	err = DL_BADQOSPARAM;
	goto error;
      tooshort:
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

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->dl_qos_offset, p->dl_qos_length))
		goto badqos;
	if (!MBLKIN(mp, p->dl_dest_addr_offset, p->dl_dest_addr_length))
		goto badaddr;
	if (dl_get_state(dl) == DL_IDLE)
		goto discard;
	if (dl_get_state(dl) != DL_IDLE)
		goto outstate;
	if (!(dl->proto.info.dl_service_mode & DL_CODLS))
		goto unsupported;
	goto notsupported;
      notsuported:
	err = DL_NOTSUPPORTED;
	goto error;
      unsupported:
	err = DL_UNSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      badaddr:
	err = DL_BADADDR;
	goto error;
      badqos:
	err = DL_BADQOSPARAM;
	goto error;
      tooshort:
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

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->dl_qos_offset, p->dl_qos_length))
		goto badqos;
	if (!MBLKIN(mp, p->dl_dest_addr_offset, p->dl_dest_addr_length))
		goto badaddr;
	if (dl_get_state(dl) == DL_IDLE)
		goto discard;
	if (dl_get_state(dl) != DL_IDLE)
		goto outstate;
	if (!(dl->proto.info.dl_service_mode & DL_CODLS))
		goto unsupported;
	goto notsupported;
      notsuported:
	err = DL_NOTSUPPORTED;
	goto error;
      unsupported:
	err = DL_UNSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      badaddr:
	err = DL_BADADDR;
	goto error;
      badqos:
	err = DL_BADQOSPARAM;
	goto error;
      tooshort:
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

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->dl_qos_offset, p->dl_qos_length))
		goto badqos;
	if (!MBLKIN(mp, p->dl_dest_addr_offset, p->dl_dest_addr_length))
		goto badaddr;
	if (dl_get_state(dl) == DL_IDLE)
		goto discard;
	if (dl_get_state(dl) != DL_IDLE)
		goto outstate;
	if (!(dl->proto.info.dl_service_mode & DL_CODLS))
		goto unsupported;
	goto notsupported;
      notsuported:
	err = DL_NOTSUPPORTED;
	goto error;
      unsupported:
	err = DL_UNSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      badaddr:
	err = DL_BADADDR;
	goto error;
      badqos:
	err = DL_BADQOSPARAM;
	goto error;
      tooshort:
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
		goto tooshort;
	if (!MBLKIN(mp, p->dl_qos_offset, p->dl_qos_length))
		goto badqos;
	if (!MBLKIN(mp, p->dl_dest_addr_offset, p->dl_dest_addr_length))
		goto badaddr;
	if (dl_get_state(dl) == DL_IDLE)
		goto discard;
	if (dl_get_state(dl) != DL_IDLE)
		goto outstate;
	if (!(dl->proto.info.dl_service_mode & DL_CODLS))
		goto unsupported;
	goto notsupported;
      notsuported:
	err = DL_NOTSUPPORTED;
	goto error;
      unsupported:
	err = DL_UNSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      badaddr:
	err = DL_BADADDR;
	goto error;
      badqos:
	err = DL_BADQOSPARAM;
	goto error;
      tooshort:
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
		goto tooshort;
	if (!MBLKIN(mp, p->dl_qos_offset, p->dl_qos_length))
		goto badqos;
	if (!MBLKIN(mp, p->dl_dest_addr_offset, p->dl_dest_addr_length))
		goto badaddr;
	if (dl_get_state(dl) == DL_IDLE)
		goto discard;
	if (dl_get_state(dl) != DL_IDLE)
		goto outstate;
	if (!(dl->proto.info.dl_service_mode & DL_CODLS))
		goto unsupported;
	goto notsupported;
      notsuported:
	err = DL_NOTSUPPORTED;
	goto error;
      unsupported:
	err = DL_UNSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      badaddr:
	err = DL_BADADDR;
	goto error;
      badqos:
	err = DL_BADQOSPARAM;
	goto error;
      tooshort:
	err = -EFAULT;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_PROMISCOFF_REQ, err);
}
static fastcall noinline __unlikely int
dl_unitdata_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_unitdata_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->dl_qos_offset, p->dl_qos_length))
		goto badqos;
	if (!MBLKIN(mp, p->dl_dest_addr_offset, p->dl_dest_addr_length))
		goto badaddr;
	if (dl_get_state(dl) == DL_IDLE)
		goto discard;
	if (dl_get_state(dl) != DL_IDLE)
		goto outstate;
	if (!(dl->proto.info.dl_service_mode & DL_CODLS))
		goto unsupported;
	goto notsupported;
      notsuported:
	err = DL_NOTSUPPORTED;
	goto error;
      unsupported:
	err = DL_UNSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      badaddr:
	err = DL_BADADDR;
	goto error;
      badqos:
	err = DL_BADQOSPARAM;
	goto error;
      tooshort:
	err = -EFAULT;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_UNITDATA_REQ, err);
}
static fastcall noinline __unlikely int
dl_udqos_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_udqos_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->dl_qos_offset, p->dl_qos_length))
		goto badqos;
	if (!MBLKIN(mp, p->dl_dest_addr_offset, p->dl_dest_addr_length))
		goto badaddr;
	if (dl_get_state(dl) == DL_IDLE)
		goto discard;
	if (dl_get_state(dl) != DL_IDLE)
		goto outstate;
	if (!(dl->proto.info.dl_service_mode & DL_CODLS))
		goto unsupported;
	goto notsupported;
      notsuported:
	err = DL_NOTSUPPORTED;
	goto error;
      unsupported:
	err = DL_UNSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      badaddr:
	err = DL_BADADDR;
	goto error;
      badqos:
	err = DL_BADQOSPARAM;
	goto error;
      tooshort:
	err = -EFAULT;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_UDQOS_REQ, err);
}

/**
 * dl_connect_req: - process a DL_CONNECT_REQ primitive
 * @dl: data link private structure
 * @q: active queue (upper write queue)
 * @mp: the DL_CONNECT_REQ primitive
 *
 * NOTE - The DL-connection release phase at each DLSAP is instantaneous, since
 * it contains only a single DL-DISCONNECT primitive.  However, the
 * corresponding protocol echanges are extended in time, with a resulting
 * transient period at each DLSAP during which the protocol's link disconnection
 * corresponds to absence of a DLC, with DLC-establishment phase unable to be
 * entered.
 *
 * The called address, calling address and responding address parameters of the
 * DL-CONNECT primitives are associated a priori with the DTE and the DCE or
 * remote DTE at the two ends of the point-to-point data link, and hence are not
 * mapped in the protocol.
 *
 * Similarly, the Quality of Service parameter set parameters are not mapped in
 * the protoocl, since only one level of QOS is available and is assumed known a
 * priori.
 *
 * - SABM/SABME/SM command transmitted when in disconnected mode, together with
 *   any retransmissions on timer expiry.
 */
static fastcall noinline __unlikely int
dl_connect_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_connect_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto efault;
	if (!MBLKIN(mp, p->dl_dest_addr_offset, p->dl_dest_addr_length))
		goto efault;
	if (!MBLKIN(mp, p->dl_qos_offset, p->dl_qos_length))
		goto efault;
	if (!(dl->proto.info.dl_service_mode & DL_CODLS))
		goto unsupported;
	if (p->dl_dest_addr_length != 0)
		goto badaddr;
	if (p->dl_qos_length != 0) {
		union {
			dl_ulong dl_qos_type;
			dl_qos_co_sel1_t sel;
			dl_qos_co_range1_t range;
		} qos;

		if (p->dl_qos_length < sizeof(qos.dl_qos_type))
			goto efault;
		bcopy(mp->b_rptr + p->dl_qos_offset, &qos, sizeof(qos.dl_qos_type));
		switch (qos.dl_qos_type) {
		case DL_QOS_CO_SEL1:
			if (p->dl_qos_length < sizeof(qos.sel))
				goto efault;
			bcopy(mp->b_rptr + p->dl_qos_offset, &qos, sizeof(qos.sel));
			/* FIXME: read values */
			break;

		case DL_QOS_CO_RANGE1:
			if (p->dl_qos_length < sizeof(qos.range))
				goto efault;
			bcopy(mp->b_rptr + p->dl_qos_offset, &qos, sizeof(qos.range));
			/* FIXME: read values */
			break;
		default:
			goto badqostype;
		}
	}
	if (p->dl_growth != 0)
		goto invalid;
	if (dl_get_state(dl) != DL_IDLE)
		goto outstate;
	dl_set_state(dl, DL_OUTCON_PENDING);
	switch (dl_get_pstate(dl)) {
	case DL_ST_ADM:
		switch (cd_get_state(dl->cd)) {
		case CD_ENABLED:
		case CD_READ_ACTIVE:
		case CD_INPUT_ALLOWED:
		case CD_OUTPUT_ACTIVE:
			/* Here we need to select the mode if it is not already set. */
			if (dl->mode == DL_MODE_AUTO) {
				/* The absolute only reason for increased modulo sizes is due to the
				 * need for an increased window size.  Therefore, the mode can be
				 * determined from the window size. */
				if (dl->k <= 7)
					dl->mode = DL_MODE_BASIC;
				if (dl->k <= 127)
					dl->mode = DL_MODE_EXTENDED;
				if (dl->k <= 32767)
					dl->mode = DL_MODE_SUPER;
				if (dl->k <= 2147483647)
					dl->mode = DL_MODE_SUPER_EXTENDED;
				if (dl->k > 2147483647) {
					dl->mode = DL_MODE_SUPER_EXTENDED;
					dl->k = 2147483647;
				}
			}
			if ((err = cd_snd_SM(dl->cd, q, mp, 1)))
				goto error;
			dl->p_flag = 1;
			mi_timer(dl->t.ack, dl->t1);
			dl->retry_count = 0;
			dl->s_flag = 0;
			dl_set_pstate(dl, DL_ST_SETUP);
			return (0);
		case CD_UNATTACHED:
		case CD_UNUSABLE:
		case CD_ENABLE_PENDING:
		case CD_DISABLE_PENDING:
		case CD_XRAY:
		case CD_NOT_AUTH:
		default:
			if ((err = dl_disconnect_ind(dl, q, mp)))
				goto error;
			dl_set_pstate(dl, DL_ST_ADM);
			return (0);
		}
	case DL_ST_D_CONN:
		/* The link is in the process of disconnecting and cannot be used yet for
		   connection.  We should refuse the connection with a temporary failure or return
		   an EAGAIN system error. It is simpler right now to do the latter. */
		goto eagain;
	default:
		mi_strlog(dl->oq, 0, SL_ERROR | SL_TRACE, "%s: invalid proto state %s",
			  __FUNCTION__, dl_pstatename(dl_get_pstate(dl)));
		goto eproto;
	}
      eagain:
	/* would pass through dl_error_reply(). */
	return dl_error_ack(dl, q, mp, DL_CONNECT_REQ, -EAGAIN);
      badqostype:
	err = DL_BADQOSTYPE;
	goto error;
      unsupported:
	err = DL_UNSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      badaddr:
	err = DL_BADADDR;
	goto error;
      badqos:
	err = DL_BADQOSPARAM;
	goto error;
      invalid:
	err = -EINVAL;
	goto errobadaddrr;
      eproto:
	err = -EPROTO;
	goto error;
      efault:
	err = -EFAULT;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_CONNECT_REQ, err);
}

/**
 * dl_connect_res: - process DL_CONNECT_RES primitive
 * @dl: data link private structure
 * @q: active queue (upper write queue)
 * @mp: the DL_CONNECT_RES primitive
 *
 * The called address, calling address and responding address parameters of the
 * DL-CONNECT primitives are associated a priori with the DTE and the DCE or
 * remote DTE at the two ends of the point-to-point data link, and hence are not
 * mapped in the protocol.
 *
 * Similarly, the Quality of Service parameter set paraemters are not mapped in
 * the protocol, since only one level of QOS is available and is assumed known
 * a priori.
 *
 * - UA response transmitted in response to SABM or SABME command received in
 *   disconnected mode.
 */
static fastcall noinline __unlikely int
dl_connect_res(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_connect_res_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto efault;
	if (!MBLKIN(mp, p->dl_qos_offset, p->dl_qos_length))
		goto efault;
	if (!(dl->proto.info.dl_service_mode & DL_CODLS))
		goto unsupported;
	if (dl_get_state(dl) != DL_INCON_PENDING)
		goto outstate;
	if (p->dl_correlation == 0)
		goto badcorr;
	if (p->dl_correlation != dl->correlation)
		goto badcorr;
	if (p->dl_token != dl->token)
		goto badtoken;
	if (p->dl_qos_length) {
		dl_qos_co_sel1_t qos;

		if (p->dl_qos_length < sizeof(qos.dl_qos_type))
			goto efault;
		bcopy(mp->b_rptr + p->dl_qos_offset, &qos.dl_qos_type, sizeof(qos.dl_qos_type));
		if (qos.dl_qos_type != DL_QOS_CO_SEL1)
			goto badqostype;
		if (p->dl_qos_length < sizeof(qos))
			goto efault;
		bcopy(mp->b_rptr + p->dl_qos_offset, &qos, sizeof(qos));
		if (qos.dl_rcv_throughput != DL_QOS_DONT_CARE)
			dl->proto.info.dl_rcv_throughput = qos.dl_rcv_throughput;
		if (qos.dl_rcv_trans_delay != DL_QOS_DONT_CARE)
			dl->proto.info.dl_rcv_trans_delay = qos.dl_rcv_trans_delay;
		if (qos.dl_xmt_throughput != DL_QOS_DONT_CARE)
			dl->proto.info.dl_xmt_throughput = qos.dl_xmt_throughput;
		if (qos.dl_xmt_trans_delay != DL_QOS_DONT_CARE)
			dl->proto.info.dl_xmt_trans_delay = qos.dl_xmt_trans_delay;
		if (qos.dl_priority != DL_QOS_DONT_CARE)
			dl->proto.info.dl_priority = qos.dl_priority;
		if (qos.dl_protection != DL_QOS_DONT_CARE)
			dl->proto.info.dl_protection = qos.dl_protection;
		if (qos.dl_residual_error != DL_QOS_DONT_CARE)
			dl->proto.info.dl_residual_error = qos.dl_residual_error;
		if (qos.dl_resilience.dl_disc_prob != DL_QOS_DONT_CARE)
			dl->proto.info.dl_resilience.dl_disc_prob = qos.dl_resilience.dl_disc_prob;
		if (qos.dl_resilience.dl_reset_prob != DL_QOS_DONT_CARE)
			dl->proto.info.dl_resilience.dl_reset_prob =
			    qos.dl_resilience.dl_reset_prob;
	}
	dl_set_state(dl, DL_CONN_RES_PENDING);
	if ((err = dl_ok_ack(dl, q, mp, DL_CONNECT_RES)))
		goto error;
	return (0);
      badqostype:
	err = DL_BADQOSTYPE;
	goto error;
      badtoken:
	err = DL_BADTOKEN;
	goto error;
      badcorr:
	err = DL_BADCORR;
	goto error;
      unsupported:
	err = DL_UNSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      badqos:
	err = DL_BADQOSPARAM;
	goto error;
      efault:
	err = -EFAULT;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_CONNECT_RES, err);
}
static fastcall noinline __unlikely int
dl_token_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_token_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->dl_qos_offset, p->dl_qos_length))
		goto badqos;
	if (!MBLKIN(mp, p->dl_dest_addr_offset, p->dl_dest_addr_length))
		goto badaddr;
	if (dl_get_state(dl) == DL_IDLE)
		goto discard;
	if (dl_get_state(dl) != DL_IDLE)
		goto outstate;
	if (!(dl->proto.info.dl_service_mode & DL_CODLS))
		goto unsupported;
	goto notsupported;
      notsuported:
	err = DL_NOTSUPPORTED;
	goto error;
      unsupported:
	err = DL_UNSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      badaddr:
	err = DL_BADADDR;
	goto error;
      badqos:
	err = DL_BADQOSPARAM;
	goto error;
      tooshort:
	err = -EFAULT;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_TOKEN_REQ, err);
}

static fastcall noinline __unlikely int
dl_data_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	int err;
	size_t dlen;

	switch (dl_get_state(dl)) {
	case DL_IDLE:
	case DL_PROV_RESET_PENDING:
		goto discard;
	case DL_DATAXFER:
		break;
	default:
		goto outstate;
	}
	dlen = msgdsize(mp);
	if (dlen < dl->proto.info.dl_min_sdu)
		goto baddata;
	if (dlen > dl->proto.info.dl_max_sdu)
		goto baddata;
	switch (dl_get_pstate(dl)) {
	case DL_ST_NORMAL:
	case DL_ST_BUSY:
	case DL_ST_REJECT:
	case DL_ST_AWAIT:
	case DL_ST_AWAIT_BUSY:
	case DL_ST_AWAIT_REJECT:
		/* FIXME: don't send these out here.  Just queue them and set the transmitter
		   wakeup.  Another condition to check is whether the flow control window is
		   closed. */
		if (dl->remote_busy == 0 && dl->tb_full == 0) {
#if 0
			/* This is what happens when they finally get sent. */
			if (dl->p_flag == 0) {
				if ((err = cd_snd_I(dl->cd, q, NULL, 1, mp)))
					goto error;
				mi_timer(dl->t.p, dl->t1);
			} else {
				if ((err = cd_snd_I(dl->cd, q, NULL, 0, mp)))
					goto error;
			}
			if (!mi_timer_running(dl->t.ack))
				mi_timer_start(dl->t.ack, dl->t1);
#endif
			bufq_queue(&dl->tb, mp);
			if (bufq_length(&dl->tb) + bufq_length(&dl->rtb) >= dl->k)
				dl->tb_full = 1;
			switch (dl_get_pstate(dl)) {
			case DL_ST_NORMAL:
			case DL_ST_BUSY:
			case DL_ST_REJECT:
				dl->txwakeup = 1;
				break;
			default:
				break;
			}
			// dl_set_pstate(dl, dl_get_state(dl));
			return (0);
		}
		err = -EBUSY;
		goto error;
	default:
		err = -EPROTO;
		goto error;
	}
      baddata:
	err = DL_BADDATA;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      discard:
	freemsg(mp);
	return (0);
      error:
	return m_error_reply(dl, q, mp, err);
}

static inline void
dl_txwakeup(queue_t *q)
{
	struct dl *dl = DL_PRIV(q);

	switch (dl_get_pstate(dl)) {
	case DL_ST_NORMAL:
	case DL_ST_BUSY:
	case DL_ST_REJECT:
		while (bufq_length(&dl->tb) > 0 && bufq_length(&dl->rtb) < dl->k) {
			mblk_t *mp, *dp;

			if (dl->remote_busy != 0)
				break;
			mp = bufq_dequeue(&dl->tb);
			if ((dp = mi_dupmsg(q, mp, BPRI_MED))) {
				if (dl->p_flag == 0) {
					/* FIXME: handle vs */
					if ((err = cd_snd_I(dl->cd, q, NULL, 1, mp))) {
						freemsg(dp);
						bufq_queue_tail(&dl->tb, mp);
						break;
					}
					mi_timer(dl->t.p, dl->t1);
					bufq_queue(&dl->rtb, mp);
					dl->vs = (dl->vs + 1) & dl->mask;
				} else {
					/* FIXME: handle vs */
					if ((err = cd_snd_I(dl->cd, q, NULL, 0, mp))) {
						freemsg(dp);
						bufq_queue_tail(&dl->tb, mp);
						break;
					}
					bufq_queue(&dl->rtb, mp);
					dl->vs = (dl->vs + 1) & dl->mask;
				}
			} else {
				bufq_queue_tail(&dl->tb, mp);
				break;
			}
		}
		break;
	case DL_ST_AWAIT:
	case DL_ST_AWAIT_BUSY:
	case DL_ST_AWAIT_REJECT:
		/* In these three states we are allowed to receive data but not to transmit it. */
		break;
	default:
		/* In other states we are not even allowed to receive data. */
		break;
	}
	return;
}

/**
 * dl_disconnect_req: - process DL_DISCONNECT_REQ primitive
 * @dl: data link private structure
 * @q: active queue (upper write queue)
 * @mp: the DL_DISCONNECT_REQ primitive
 *
 * The Reason parameter in every DL-DISCONNECT request and indication primitive
 * is "reason unspecified".
 *
 * - DISC command transmitted when in information transfer phase, together with
 *   any retransmissions on timer expiry.
 *
 * - DM response transmitted in response to SABM/SABME/SM command received
 *   received in disconnected mode (rejection of DLC establishment).
 */
static fastcall noinline __unlikely int
dl_disconnect_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_disconnect_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto efault;
	if (!(dl->proto.info.dl_service_mode & DL_CODLS))
		goto unsupported;
	switch (dl_get_state(dl)) {
	case DL_IDLE:
		goto discard;
	case DL_OUTCON_PENDING:
		dl_set_state(dl, DL_DISCON8_PENDING);
		break;
	case DL_INCON_PENDING:
		if (p->dl_correlation == 0)
			goto badcorr;
		if (p->dl_correlation != dl->correlation)
			goto badcorr;
		dl_set_state(dl, DL_DISCON9_PENDING);
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
	/* dl_ok_ack does all the work */
	if ((err = dl_ok_ack(dl, q, mp, DL_DISCONNECT_REQ)))
		goto error;
	return (0);
      badcorr:
	err = DL_BADCORR;
	goto error;
      unsupported:
	err = DL_UNSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      efault:
	err = -EFAULT;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_DISCONNECT_REQ, err);
      discard:
	freemsg(mp);
	return (0);
}

/**
 * dl_reset_req: - process DL_RESET_REQ primitive
 * @dl: data link private structure
 * @q: active queue (upper write queue)
 * @mp: the DL_RESET_REQ primitive
 *
 * SABM/SABME/SM command transmitted.  The first occurence of one of these
 * frames during normal data transfer together with any retransmissions required
 * by the X.25 LAPB procedures, maps to the DL-RESET request or indication
 * primitive.  Subsequen occurences of other frames from this set before either
 * the link reset is completed or the link is disconnected do not map to any DLS
 * primitive.
 */
static fastcall noinline __unlikely int
dl_reset_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	int err;

	if (!(dl->proto.info.dl_service_mode & DL_CODLS))
		goto unsupported;
	if (dl_get_state(dl) == DL_IDLE)
		goto discard;
	if (dl_get_state(dl) != DL_DATAXFER)
		goto outstate;
	switch (dl_get_pstate(dl)) {
	case DL_ST_RESET_WAIT:
		/* local provider reset */
		if (dl->s_flag == 0) {
			if ((err = cd_snd_SM(dl->cd, q, mp, 1)))
				goto error;
			dl->p_flag = 1;
			mi_timer(dl->t.ack, dl->t1);
			dl->retry_count = 0;
			dl_set_pstate(dl, DL_ST_RESET);
			dl_set_state(dl, DL_USER_RESET_PENDING);
			return (0);
		}
		if ((err = cd_snd_UA(dl->cd, q, NULL, dl->f_flag)))
			goto error;
		dl->vs = 0;
		dl->vr = 0;
		dl->retry_count = 0;
		dl->p_flag = 0;
		dl->remote_busy = 0;
		dl_set_pstate(dl, DL_ST_D_CONN);
		/* fall thru */
	case DL_ST_D_CONN:
		/* protection for second pass */
		dl_set_state(dl, DL_USER_RESET_PENDING);
		if ((err = dl_reset_con(dl, q, mp)))
			goto error;
		return (0);
	case DL_ST_NORMAL:
	case DL_ST_BUSY:
	case DL_ST_REJECT:
	case DL_ST_AWAIT:
	case DL_ST_AWAIT_BUSY:
	case DL_ST_AWAIT_REJECT:
		if ((err = cd_snd_SM(dl->cd, q, mp, 1)))
			goto error;
		dl->p_flag = 1;
		mi_timer_stop(dl->t.ack);
		mi_timer_stop(dl->t.p);
		mi_timer_stop(dl->t.rej);
		mi_timer_stop(dl->t.busy);
		dl->retry_count = 0;
		dl->s_flag = 0;
		mi_timer(dl->t.ack, dl->t1);
		dl_set_pstate(dl, DL_ST_RESET);
		return (0);
	}
      unsupported:
	err = DL_UNSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_RESET_REQ, err);
      discard:
	freemsg(mp);
	return (0);
}

/**
 * dl_reset_res: - process DL_RESET_RES primitive
 * @dl: data link private structure
 * @q: active queue (upper write queue)
 * @mp: the DL_RESET_RES primitive
 *
 * - Following a DL-RESET indication.
 *
 * - UA response transmitted or received, as appropriate, to complete a link
 *   reset.
 *
 * - Time-out waiting for UA response, after sending UA response to a colliding
 *   SABM/SABME/SM command received.
 *
 * The correspondence between these primitives, marking completion of DLC
 * resetting, and the protocol frames or timeouts uses the earliest externally
 * observable real-world events with which the abstract primitives can be
 * associated.  The significance of the primitives in the CO-DLS is that they
 * separate a period when DL-DATA primitive cannot occur from the following
 * period when DL-DATA primitives are again possible: the mapping specified
 * simply relates this to the equivalent separation between X.25 LAPB link
 * resetting procedure, during which no information transfer occurs, and the
 * resumption of the capability for normal information transfer on completion of
 * the link reset.  Within an implementation, it may be convenient to consider
 * representations of the primitives as occurring either earlier or later.  If
 * earlier, there will be a period during which DL-DATA request primitives as
 * occurring either earlier or later.  If earlier, there will be a period during
 * which DL-DATA request primitives cannot be issued at the DLSAP, since the
 * X.25 LAPB procedures prevent transmission of I-frames; if later, there will
 * be a period following completion of the X.25 LAPB link reset during which
 * I-frames are not transmitted, because the local implementation is not ready.
 * Such an implementation-based view is not precluded, since it is outside the
 * scope of OSI standardization.
 */
static fastcall noinline __unlikely int
dl_reset_res(struct dl *dl, queue_t *q, mblk_t *mp)
{
	int err;

	if (!(dl->proto.info.dl_service_mode & DL_CODLS))
		goto unsupported;
	if (dl_get_state(dl) == DL_IDLE)
		goto discard;
	if (dl_get_state(dl) != DL_PROV_RESET_PENDING)
		goto outstate;
	dl_set_state(dl, DL_RESET_RES_PENDING);
	/* dl_ok_ack does all the work */
	if ((err = dl_ok_ack(dl, q, mp, DL_RESET_RES)))
		goto error;
	return (0);
      outstate:
	err = DL_OUTSTATE;
	goto error;
      discard:
	freemsg(mp);
	return (0);
      unsupported:
	err = DL_UNSUPPORTED;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_RESET_RES, err);
}
static fastcall noinline __unlikely int
dl_reply_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_reply_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->dl_qos_offset, p->dl_qos_length))
		goto badqos;
	if (!MBLKIN(mp, p->dl_dest_addr_offset, p->dl_dest_addr_length))
		goto badaddr;
	if (dl_get_state(dl) == DL_IDLE)
		goto discard;
	if (dl_get_state(dl) != DL_IDLE)
		goto outstate;
	if (!(dl->proto.info.dl_service_mode & DL_CODLS))
		goto unsupported;
	goto notsupported;
      notsuported:
	err = DL_NOTSUPPORTED;
	goto error;
      unsupported:
	err = DL_UNSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      badaddr:
	err = DL_BADADDR;
	goto error;
      badqos:
	err = DL_BADQOSPARAM;
	goto error;
      tooshort:
	err = -EFAULT;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_REPLY_REQ, err);
}
static fastcall noinline __unlikely int
dl_reply_update_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_reply_update_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->dl_qos_offset, p->dl_qos_length))
		goto badqos;
	if (!MBLKIN(mp, p->dl_dest_addr_offset, p->dl_dest_addr_length))
		goto badaddr;
	if (dl_get_state(dl) == DL_IDLE)
		goto discard;
	if (dl_get_state(dl) != DL_IDLE)
		goto outstate;
	if (!(dl->proto.info.dl_service_mode & DL_CODLS))
		goto unsupported;
	goto notsupported;
      notsuported:
	err = DL_NOTSUPPORTED;
	goto error;
      unsupported:
	err = DL_UNSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      badaddr:
	err = DL_BADADDR;
	goto error;
      badqos:
	err = DL_BADQOSPARAM;
	goto error;
      tooshort:
	err = -EFAULT;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_REPLY_UPDATE_REQ, err);
}
static fastcall noinline __unlikely int
dl_xid_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_xid_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->dl_qos_offset, p->dl_qos_length))
		goto badqos;
	if (!MBLKIN(mp, p->dl_dest_addr_offset, p->dl_dest_addr_length))
		goto badaddr;
	if (dl_get_state(dl) == DL_IDLE)
		goto discard;
	if (dl_get_state(dl) != DL_IDLE)
		goto outstate;
	if (!(dl->proto.info.dl_service_mode & DL_CODLS))
		goto unsupported;
	goto notsupported;
      notsuported:
	err = DL_NOTSUPPORTED;
	goto error;
      unsupported:
	err = DL_UNSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      badaddr:
	err = DL_BADADDR;
	goto error;
      badqos:
	err = DL_BADQOSPARAM;
	goto error;
      tooshort:
	err = -EFAULT;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_XID_REQ, err);
}
static fastcall noinline __unlikely int
dl_xid_res(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_xid_res_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->dl_qos_offset, p->dl_qos_length))
		goto badqos;
	if (!MBLKIN(mp, p->dl_dest_addr_offset, p->dl_dest_addr_length))
		goto badaddr;
	if (dl_get_state(dl) == DL_IDLE)
		goto discard;
	if (dl_get_state(dl) != DL_IDLE)
		goto outstate;
	if (!(dl->proto.info.dl_service_mode & DL_CODLS))
		goto unsupported;
	goto notsupported;
      notsuported:
	err = DL_NOTSUPPORTED;
	goto error;
      unsupported:
	err = DL_UNSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      badaddr:
	err = DL_BADADDR;
	goto error;
      badqos:
	err = DL_BADQOSPARAM;
	goto error;
      tooshort:
	err = -EFAULT;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_XID_RES, err);
}
static fastcall noinline __unlikely int
dl_test_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_test_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->dl_qos_offset, p->dl_qos_length))
		goto badqos;
	if (!MBLKIN(mp, p->dl_dest_addr_offset, p->dl_dest_addr_length))
		goto badaddr;
	if (dl_get_state(dl) == DL_IDLE)
		goto discard;
	if (dl_get_state(dl) != DL_IDLE)
		goto outstate;
	if (!(dl->proto.info.dl_service_mode & DL_CODLS))
		goto unsupported;
	goto notsupported;
      notsuported:
	err = DL_NOTSUPPORTED;
	goto error;
      unsupported:
	err = DL_UNSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      badaddr:
	err = DL_BADADDR;
	goto error;
      badqos:
	err = DL_BADQOSPARAM;
	goto error;
      tooshort:
	err = -EFAULT;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_TEST_REQ, err);
}
static fastcall noinline __unlikely int
dl_test_res(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_test_res_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->dl_qos_offset, p->dl_qos_length))
		goto badqos;
	if (!MBLKIN(mp, p->dl_dest_addr_offset, p->dl_dest_addr_length))
		goto badaddr;
	if (dl_get_state(dl) == DL_IDLE)
		goto discard;
	if (dl_get_state(dl) != DL_IDLE)
		goto outstate;
	if (!(dl->proto.info.dl_service_mode & DL_CODLS))
		goto unsupported;
	goto notsupported;
      notsuported:
	err = DL_NOTSUPPORTED;
	goto error;
      unsupported:
	err = DL_UNSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      badaddr:
	err = DL_BADADDR;
	goto error;
      badqos:
	err = DL_BADQOSPARAM;
	goto error;
      tooshort:
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

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->dl_qos_offset, p->dl_qos_length))
		goto badqos;
	if (!MBLKIN(mp, p->dl_dest_addr_offset, p->dl_dest_addr_length))
		goto badaddr;
	if (dl_get_state(dl) == DL_IDLE)
		goto discard;
	if (dl_get_state(dl) != DL_IDLE)
		goto outstate;
	if (!(dl->proto.info.dl_service_mode & DL_CODLS))
		goto unsupported;
	goto notsupported;
      notsuported:
	err = DL_NOTSUPPORTED;
	goto error;
      unsupported:
	err = DL_UNSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      badaddr:
	err = DL_BADADDR;
	goto error;
      badqos:
	err = DL_BADQOSPARAM;
	goto error;
      tooshort:
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
		goto tooshort;
	if (!MBLKIN(mp, p->dl_qos_offset, p->dl_qos_length))
		goto badqos;
	if (!MBLKIN(mp, p->dl_dest_addr_offset, p->dl_dest_addr_length))
		goto badaddr;
	if (dl_get_state(dl) == DL_IDLE)
		goto discard;
	if (dl_get_state(dl) != DL_IDLE)
		goto outstate;
	if (!(dl->proto.info.dl_service_mode & DL_CODLS))
		goto unsupported;
	goto notsupported;
      notsuported:
	err = DL_NOTSUPPORTED;
	goto error;
      unsupported:
	err = DL_UNSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      badaddr:
	err = DL_BADADDR;
	goto error;
      badqos:
	err = DL_BADQOSPARAM;
	goto error;
      tooshort:
	err = -EFAULT;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_SET_PHYS_ADDR_REQ, err);
}
static fastcall noinline __unlikely int
dl_get_statistics_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_get_statistics_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->dl_qos_offset, p->dl_qos_length))
		goto badqos;
	if (!MBLKIN(mp, p->dl_dest_addr_offset, p->dl_dest_addr_length))
		goto badaddr;
	if (dl_get_state(dl) == DL_IDLE)
		goto discard;
	if (dl_get_state(dl) != DL_IDLE)
		goto outstate;
	if (!(dl->proto.info.dl_service_mode & DL_CODLS))
		goto unsupported;
	goto notsupported;
      notsuported:
	err = DL_NOTSUPPORTED;
	goto error;
      unsupported:
	err = DL_UNSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      badaddr:
	err = DL_BADADDR;
	goto error;
      badqos:
	err = DL_BADQOSPARAM;
	goto error;
      tooshort:
	err = -EFAULT;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_GET_STATISTICS_REQ, err);
}
static fastcall noinline __unlikely int
dl_wrongway_ind(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_ulong *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->dl_qos_offset, p->dl_qos_length))
		goto badqos;
	if (!MBLKIN(mp, p->dl_dest_addr_offset, p->dl_dest_addr_length))
		goto badaddr;
	if (dl_get_state(dl) == DL_IDLE)
		goto discard;
	if (dl_get_state(dl) != DL_IDLE)
		goto outstate;
	if (!(dl->proto.info.dl_service_mode & DL_CODLS))
		goto unsupported;
	goto notsupported;
      notsuported:
	err = DL_NOTSUPPORTED;
	goto error;
      unsupported:
	err = DL_UNSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      badaddr:
	err = DL_BADADDR;
	goto error;
      badqos:
	err = DL_BADQOSPARAM;
	goto error;
      tooshort:
	err = -EFAULT;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, *p, err);
}
static fastcall noinline __unlikely int
dl_other_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_ulong *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->dl_qos_offset, p->dl_qos_length))
		goto badqos;
	if (!MBLKIN(mp, p->dl_dest_addr_offset, p->dl_dest_addr_length))
		goto badaddr;
	if (dl_get_state(dl) == DL_IDLE)
		goto discard;
	if (dl_get_state(dl) != DL_IDLE)
		goto outstate;
	if (!(dl->proto.info.dl_service_mode & DL_CODLS))
		goto unsupported;
	goto notsupported;
      notsuported:
	err = DL_NOTSUPPORTED;
	goto error;
      unsupported:
	err = DL_UNSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      badaddr:
	err = DL_BADADDR;
	goto error;
      badqos:
	err = DL_BADQOSPARAM;
	goto error;
      tooshort:
	err = -EFAULT;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, *p, err);
}

/*
 * --------------------------------------------------------------------------
 *
 * CDI PROVIDER TO CDI USER RECEIVED PRIMITIVES
 *
 * --------------------------------------------------------------------------
 */
static fastcall noinline __unlikely int
cd_info_ack(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_info_ack_t *p = (typeof(p)) mp->b_rptr;
	int err = -EPROTO;

	if (cd->proto.info.cd_primitive == 0) {
		/* This is the first response to a CD_INFO_ACK. */
		cd->proto.info.cd_primitive = CD_INFO_ACK;
		cd->proto.info.cd_state = cd_set_state(cd, p->cd_state);
		if (p->cd_max_sdu != DL_UNKNOWN) {
			cd->proto.info.cd_max_sdu = p->cd_max_sdu;
			if (cd->dl->proto.info.dl_max_sdu > p->cd_max_sdu)
				cd->dl->proto.info.dl_max_sdu = p->cd_max_sdu;
		}
		if (p->cd_min_sdu != DL_UNKNOWN) {
			cd->proto.info.cd_min_sdu = p->cd_min_sdu;
			if (cd->dl->proto.info.dl_min_sdu > p->cd_min_sdu)
				cd->dl->proto.info.dl_min_sdu = p->cd_min_sdu;
		}
		if (p->cd_class = !DL_UNKNOWN) {
			cd->proto.info.cd_class = p->cd_class;
			if (p->cd_class != CD_HDLC)
				goto error;
		}
		if (p->cd_duplex != DL_UNKNOWN) {
			cd->proto.info.cd_duplex = p->cd_duplex;
			if (p->cd_duplex != CD_FULLDUPLEX)
				goto error;
		}
		if (p->cd_output_style != DL_UNKNOWN) {
			cd->proto.info.cd_output_style = p->cd_output_style;
			if (p->cd_duplex != CD_UNACKEDOUTPUT)
				goto error;
		}
		if (p->cd_features != DL_UNKNOWN) {
			cd->proto.info.cd_features = p->cd_features;
			if ((p->cd_features & (CD_AUTOALLOW | CD_CANREAD)) !=
			    (CD_AUTOALLOW | CD_CANREAD))
				goto error;
		}
		if (p->cd_addr_length != DL_UNKNOWN) {
			cd->proto.info.cd_addr_length = p->cd_addr_length;
			if (p->cd_addr_length != 0)
				goto error;
		}
		cd->proto.info.cd_ppa_style = p->cd_ppa_style;
		switch (p->cd_ppa_style) {
		case CD_STYLE1:
			cd->dl->proto.info.dl_provider_style = DL_STYLE1;
			cd->dl->state = DL_UNBOUND;
			cd->dl->oldstate = DL_UNBOUND;
			cd->dl->proto.info.dl_current_state = DL_UNBOUND;
			break;
		case CD_STYLE2:
			cd->dl->proto.info.dl_provider_style = DL_STYLE2;
			cd->dl->state = DL_UNATTACHED;
			cd->dl->oldstate = DL_UNATTACHED;
			cd->dl->proto.info.dl_current_state = DL_UNATTACHED;
			break;
		default:
			goto error;
		}
		cd->dl->proto.info.dl_primitive = DL_INFO_ACK;
	} else {
		cd->proto.info.cd_primitive = CD_INFO_ACK;
		cd->proto.info.cd_state = cd_set_state(cd, p->cd_state);
		cd->proto.info.cd_max_sdu = p->cd_max_sdu;
		cd->proto.info.cd_min_sdu = p->cd_min_sdu;
		cd->proto.info.cd_class = p->cd_class;
		if (p->cd_class != CD_HDLC)
			goto error;
		cd->proto.info.cd_duplex = p->cd_duplex;
		if (p->cd_duplex != CD_FULLDUPLEX)
			goto error;
		cd->proto.info.cd_output_style = p->cd_output_style;
		if (p->cd_duplex != CD_UNACKEDOUTPUT)
			goto error;
		cd->proto.info.cd_features = p->cd_features;
		if ((p->cd_features & (CD_AUTOALLOW | CD_CANREAD)) != (CD_AUTOALLOW | CD_CANREAD))
			goto error;
		cd->proto.info.cd_addr_length = p->cd_addr_length;
		if (p->cd_addr_length != 0)
			goto error;
		cd->proto.info.cd_ppa_style = p->cd_ppa_style;
		cd->dl->proto.info.dl_primitive = DL_INFO_ACK;
	}
	qenable(WR(cd->dl->oq));
	freemsg(mp);
	return (0);
      error:
	return m_error_reply(cd->dl, q, mp, err);
}
static fastcall noinline __unlikely int
cd_ok_ack(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_ok_ack_t *p = (typeof(p)) mp->b_rptr;

	switch (cd_get_state(cd)) {
	case CD_ATTACH_PENDING:
		cd_set_state(cd, CD_DISABLED);
		return dl_ok_ack(cd->dl, q, mp, DL_ATTACH_REQ);
	case CD_DETACH_PENDING:
		cd_set_state(cd, CD_UNATTACHED);
		return dl_ok_ack(cd->dl, q, mp, DL_DETACH_REQ);
	case CD_ENABLE_PENDING:
		cd_set_state(cd, CD_ENABLED);
		return dl_bind_ack(cd->dl, q, mp);
	case CD_DISABLE_PENDING:
		cd_set_state(cd, CD_DISABLED);
		return dl_ok_ack(cd->dl, q, mp, DL_UNBIND_REQ);
	}
}
dl_long
dlerror(cd_ulong errno, cd_ulong explanation)
{
	switch (errno) {
	case 0:
		return (0);
	case CD_BADADDRESS:
		return (DL_BADADDR);
	case CD_BADADDRTYPE:
		return (DL_BADADDR);
	case CD_BADDIAL:
		return (DL_BADADDR);
	case CD_BADDIALTYPE:
		return (DL_BADADDR);
	case CD_BADDISPOSAL:
		return (DL_BADQOSPARAM);
	case CD_BADFRAME:
		return (DL_BADDATA);
	case CD_BADPPA:
		return (DL_BADPPA);
	case CD_BADPRIM:
		return (DL_BADPRIM);
	case CD_DISC:
		return (DL_NOTINIT);
	case CD_EVENT:
		return (-EPIPE);
	case CD_FATALERR:
		return (-EPROTO);
	case CD_INITFAILED:
		return (DL_INITFAILED);
	case CD_NOTSUPP:
		return (DL_NOTSUPPORTED);
	case CD_OUTSTATE:
		return (DL_OUTSTATE);
	case CD_PROTOSHORT:
		return (-EFAULT);
	case CD_READTIMEOUT:
		return (-ETIMEDOUT);
	case CD_SYSERR:
		return (-explanation);
	case CD_WRITEFAIL:
		return (DL_UNDELIVERABLE);
	default:
		return (-EFAULT);
	}
}
static fastcall noinline __unlikely int
cd_error_ack(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_error_ack_t *p = (typeof(p)) mp->b_rptr;

	switch (cd_get_state(cd)) {
	case CD_ATTACH_PENDING:
		cd_set_state(cd, CD_UNATTACHED);
		return dl_error_ack(cd->dl, q, mp, DL_ATTACH_REQ,
				    dlerror(p->cd_errno, p->cd_explanation));
	case CD_DETACH_PENDING:
		cd_set_state(cd, CD_DISABLED);
		return dl_error_ack(cd->dl, q, mp, DL_DETACH_REQ,
				    dlerror(p->cd_errno, p->cd_explanation));
	case CD_ENABLE_PENDING:
		cd_set_state(cd, CD_DISABLED);
		return dl_error_ack(cd->dl, q, mp, DL_BIND_REQ,
				    dlerror(p->cd_errno, p->cd_explanation));
	case CD_DISABLE_PENDING:
		cd_set_state(cd, CD_ENABLED);
		return dl_error_ack(cd->dl, q, mp, DL_UNBIND_REQ,
				    dlerror(p->cd_errno, p->cd_explanation));
	}
}
static fastcall noinline __unlikely int
cd_enable_con(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_enable_con_t *p = (typeof(p)) mp->b_rptr;
}
static fastcall noinline __unlikely int
cd_disable_con(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_disable_con_t *p = (typeof(p)) mp->b_rptr;
}
static fastcall noinline __unlikely int
cd_error_ind(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_error_ind_t *p = (typeof(p)) mp->b_rptr;
}
static fastcall noinline __unlikely int
cd_unitdata_ack(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_unitdata_ack_t *p = (typeof(p)) mp->b_rptr;
}
static fastcall noinline __unlikely int
cd_unitdata_ind(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_unitdata_ind_t *p = (typeof(p)) mp->b_rptr;

	/* Address type should be CD_IMPLICIT and source and destination addresses are contained in 
	   the message.  The priority doesn't count at this point.  Really all we need is the
	   message. */
	return dl_rcv_msg(cd->dl, q, mp, mp->b_cont);
}
static fastcall noinline __unlikely int
cd_bad_frame_ind(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_bad_frame_ind_t *p = (typeof(p)) mp->b_rptr;
}
static fastcall noinline __unlikely int
cd_modem_sig_ind(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_modem_sig_ind_t *p = (typeof(p)) mp->b_rptr;
}
static fastcall noinline __unlikely int
cd_wrongway_ack(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_ulong *p = (typeof(p)) mp->b_rptr;
}
static fastcall noinline __unlikely int
cd_other_ind(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_ulong *p = (typeof(p)) mp->b_rptr;
}

/*
 * --------------------------------------------------------------------------
 *
 * STREAMS MESSAGE HANDLING
 *
 * -------------------------------------------------------------------------
 */
static fastcall noinline __unlikely int
dl_m_data(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);	/* already locked */

	return dl_data_req(dl, q, mp);
}
static fastcall noinline __unlikely int
dl_m_proto(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_ulong prim;
	int rtn = -EFAULT;

	if (!MBLKIN(mp, 0, sizeof(prim)))
		goto done;
	prim = *(dl_ulong *) mp->b_rptr;
	dl_save_state(dl);
	switch (prim) {
	case DL_INFO_REQ:
		rtn = dl_info_req(dl, q, mp);
		break;
	case DL_ATTACH_REQ:
		rtn = dl_attach_req(dl, q, mp);
		break;
	case DL_DETACH_REQ:
		rtn = dl_detach_req(dl, q, mp);
		break;
	case DL_BIND_REQ:
		rtn = dl_bind_req(dl, q, mp);
		break;
	case DL_UNBIND_REQ:
		rtn = dl_unbind_req(dl, q, mp);
		break;
	case DL_SUBS_BIND_REQ:
		rtn = dl_subs_bind_req(dl, q, mp);
		break;
	case DL_SUBS_UNBIND_REQ:
		rtn = dl_subs_unbind_req(dl, q, mp);
		break;
	case DL_ENABMULTI_REQ:
		rtn = dl_enabmulti_req(dl, q, mp);
		break;
	case DL_DISABMULTI_REQ:
		rtn = dl_disabmulti_req(dl, q, mp);
		break;
	case DL_PROMISCON_REQ:
		rtn = dl_promiscon_req(dl, q, mp);
		break;
	case DL_PROMISCOFF_REQ:
		rtn = dl_promiscoff_req(dl, q, mp);
		break;
	case DL_UNITDATA_REQ:
		rtn = dl_unitdata_req(dl, q, mp);
		break;
	case DL_UDQOS_REQ:
		rtn = dl_udqos_req(dl, q, mp);
		break;
	case DL_CONNECT_REQ:
		rtn = dl_connect_req(dl, q, mp);
		break;
	case DL_CONNECT_RES:
		rtn = dl_conenct_res(dl, q, mp);
		break;
	case DL_TOKEN_REQ:
		rtn = dl_token_req(dl, q, mp);
		break;
	case DL_DISCONNECT_REQ:
		rtn = dl_disconnect_req(dl, q, mp);
		break;
	case DL_RESET_REQ:
		rtn = dl_reset_req(dl, q, mp);
		break;
	case DL_RESET_RES:
		rtn = dl_reset_res(dl, q, mp);
		break;
	case DL_DATA_ACK_REQ:
		rtn = dl_data_ack_req(dl, q, mp);
		break;
	case DL_REPLY_REQ:
		rtn = dl_reply_req(dl, q, mp);
		break;
	case DL_REPLY_UPDATE_REQ:
		rtn = dl_reply_update_req(dl, q, mp);
		break;
	case DL_XID_REQ:
		rtn = dl_xid_req(dl, q, mp);
		break;
	case DL_XID_RES:
		rtn = dl_xid_res(dl, q, mp);
		break;
	case DL_TEST_REQ:
		rtn = dl_test_req(dl, q, mp);
		break;
	case DL_TEST_RES:
		rtn = dl_test_res(dl, q, mp);
		break;
	case DL_PHYS_ADDR_REQ:
		rtn = dl_phys_addr_req(dl, q, mp);
		break;
	case DL_SET_PHYS_ADDR_REQ:
		rtn = dl_set_phys_addr_req(dl, q, mp);
		break;
	case DL_GET_STATISTICS_REQ:
		rtn = dl_get_statistics_req(dl, q, mp);
		break;
	case DL_INFO_ACK:
	case DL_BIND_ACK:
	case DL_OK_ACK:
	case DL_ERROR_ACK:
	case DL_SUBS_BIND_ACK:
	case DL_TOKEN_ACK:
	case DL_PHYS_ADDR_ACK:
	case DL_GET_STATISTICS_ACK:
	case DL_UNITDATA_IND:
	case DL_UDERROR_IND:
	case DL_CONNECT_IND:
	case DL_CONNECT_CON:
	case DL_DISCONNECT_IND:
	case DL_RESET_IND:
	case DL_RESET_CON:
	case DL_DATA_ACK_IND:
	case DL_DATA_ACK_STATUS_IND:
	case DL_REPLY_IND:
	case DL_REPLY_STATUS_IND:
	case DL_REPLY_UPDATE_STATUS_IND:
	case DL_XID_IND:
	case DL_XID_CON:
	case DL_TEST_IND:
	case DL_TEST_CON:
		rtn = dl_wrongway_ind(dl, q, mp);
		break;
	default:
		rtn = dl_other_req(dl, q, mp);
		break;
	}
      done:
	if (rtn)
		dl_restore_state(dl);
	return (0);
}
static fastcall noinline __unlikely int
dl_m_ctl(queue_t *q, mblk_t *mp)
{
	/* No safe path for now... */
	return dl_m_proto(q, mp);
}
static fastcall noinline __unlikely int
dl_m_sig(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	int rtn;

	if (!mi_timer_valid(mp))
		goto done;
	dl_save_state(dl);
	switch (*(int *) mp->b_rptr) {
	case DL_TIMER_ACK:
		mi_strlog(q, STRLOGTO, SL_TRACE, "-> ACK TIMEOUT <-");
		rtn = dl_timeout_ack(dl, q);
		break;
	case DL_TIMER_P:
		mi_strlog(q, STRLOGTO, SL_TRACE, "-> P TIMEOUT <-");
		rtn = dl_timeout_p(dl, q);
		break;
	case DL_TIMER_REJ:
		mi_strlog(q, STRLOGTO, SL_TRACE, "-> REJ TIMEOUT <-");
		rtn = dl_timeout_rej(dl, q);
		break;
	case DL_TIMER_BUSY:
		mi_strlog(q, STRLOGTO, SL_TRACE, "-> BUSY TIMEOUT <-");
		rtn = dl_timeout_busy(dl, q);
		break;
	default:
		mi_strlog(q, STRLOGTO, SL_TRACE, "-> undefined TIMEOUT <-");
		goto discard;
	}
	if (rtn) {
		dl_restore_state(dl);
		rtn = mi_timer_requeue(mp) ? rtn : 0;
	}
	return (rtn);
      done:
	return (0);
      discard:
	freemsg(mp);
	return (0);
}
static fastcall noinline __unlikely int
dl_m_rse(queue_t *q, mblk_t *mp)
{
	freemsg(mp);
	return (0);
}
static fastcall noinline __unlikely int
dl_m_flush(queue_t *q, mblk_t *mp)
{
	if (mp->b_rptr[0] & FLUSHW) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
	}
	putnext(q, mp);
	return (0);
}
static fastcall noinline __unlikely int
dl_m_ioctl(queue_t *q, mblk_t *mp)
{
	putnext(q, mp);
	return (0);
}
static fastcall noinline __unlikely int
dl_m_iocdata(queue_t *q, mblk_t *mp)
{
	putnext(q, mp);
	return (0);
}
static fastcall noinline __unlikely int
dl_m_read(queue_t *q, mblk_t *mp)
{
	putnext(q, mp);
	return (0);
}
static fastcall noinline __unlikely int
dl_m_other(queue_t *q, mblk_t *mp)
{
	mi_strlog(q, 0, SL_ERROR, "%s; bad message type on write queue %d\n", __FUNCTION__,
		  (int) DB_TYPE(mp));
	putnext(q, mp);
	return (0);
}

static fastcall noinline __unlikely int
cd_m_data(queue_t *q, mblk_t *mp)
{
	if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}
static fastcall noinline __unlikely int
cd_m_proto(queue_t *q, mblk_t *mp)
{
	struct cd *cd = CD_PRIV(q);	/* already locked */
	cd_ulong prim;
	int rtn = -EFAULT;

	if (!MBLKIN(mp, 0, sizeof(prim)))
		goto done;
	prim = *(cd_ulong *) mp->b_rptr;
	cd_save_state(cd);
	switch (prim) {
	case CD_INFO_ACK:
		rtn = cd_info_ack(cd, q, mp);
		break;
	case CD_OK_ACK:
		rtn = cd_ok_ack(cd, q, mp);
		break;
	case CD_ERROR_ACK:
		rtn = cd_error_ack(cd, q, mp);
		break;
	case CD_ENABLE_CON:
		rtn = cd_enable_con(cd, q, mp);
		break;
	case CD_DISABLE_CON:
		rtn = cd_disable_con(cd, q, mp);
		break;
	case CD_ERROR_IND:
		rtn = cd_error_ind(cd, q, mp);
		break;
	case CD_UNITDATA_ACK:
		rtn = cd_unitdata_ack(cd, q, mp);
		break;
	case CD_UNITDATA_IND:
		rtn = cd_unitdata_ind(cd, q, mp);
		break;
	case CD_BAD_FRAME_IND:
		rtn = cd_bad_frame_ind(cd, q, mp);
		break;
	case CD_MODEM_SIG_IND:
		rtn = cd_modem_sig_ind(cd, q, mp);
		break;
	case CD_INFO_REQ:
	case CD_ATTACH_REQ:
	case CD_DETACH_REQ:
	case CD_ENABLE_REQ:
	case CD_DISABLE_REQ:
	case CD_ALLOW_INPUT_REQ:
	case CD_READ_REQ:
	case CD_UNITDATA_REQ:
	case CD_WRITE_READ_REQ:
	case CD_HALT_INPUT_REQ:
	case CD_ABORT_OUTPUT_REQ:
	case CD_MUX_NAME_REQ:
	case CD_MODEM_SIG_REQ:
	case CD_MODEM_SIG_POLL:
		rtn = cd_wrongway_req(cd, q, mp);
		break;
	default:
		rtn = cd_other_ind(cd, q, mp);
		break;
	}
      done:
	if (rtn)
		cd_restore_state(cd);
	return (0);
}
static fastcall noinline __unlikely int
cd_m_sig(queue_t *q, mblk_t *mp)
{
	if (!MBLKIN(mp, 0, sizeof(int))) {
		if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
			putnext(q, mp);
			return (0);
		}
		return (-EBUSY);
	}
	freemsg(mp);		/* process when we have timers */
	return (0);
}
static fastcall noinline __unlikely int
cd_m_ctl(queue_t *q, mblk_t *mp)
{
	/* Normal path _is_ safe path. */
	return cd_m_proto(q, mp);
}
static fastcall noinline __unlikely int
cd_m_rse(queue_t *q, mblk_t *mp)
{
	freemsg(mp);
	return (0);
}
static fastcall noinline __unlikely int
cd_m_flush(queue_t *q, mblk_t *mp)
{
	if (mp->b_rptr[0] & FLUSHR) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
	}
	putnext(q, mp);
	return (0);
}
static fastcall noinline __unlikely int
cd_m_error(queue_t *q, mblk_t *mp)
{
	cd_set_state(tp, CD_NOT_AUTH);
	flushq(q, FLUSHALL);
	flushq(WR(q), FLUSHALL);
	putnext(q, mp);
	return (0);
}
static fastcall noinline __unlikely int
cd_m_hangup(queue_t *q, mblk_t *mp)
{
	putnext(q, mp);
	return (0);
}
static fastcall noinline __unlikely int
cd_m_setopts(queue_t *q, mblk_t *mp)
{
	putnext(q, mp);
	return (0);
}
static fastcall noinline __unlikely int
cd_m_copy(queue_t *q, mblk_t *mp)
{
	putnext(q, mp);
	return (0);
}
static fastcall noinline __unlikely int
cd_m_iocack(queue_t *q, mblk_t *mp)
{
	putnext(q, mp);
	return (0);
}
static fastcall noinline __unlikely int
cd_m_other(queue_t *q, mblk_t *mp)
{
	mi_strlog(q, 0, SL_ERROR, "%s: unrecognized STREAMS message on read queue, %d",
		  __FUNCTION__, (int) DB_TYPE(mp));
	putnext(q, mp);
	return (0);
}

static fastcall inline int
dl_msg_put(queue_t *q, mblk_t *mp)
{
	if (likely(DB_TYPE(mp) == M_DATA))
		return (-EAGAIN);	/* fast path for data */
	if (likely(DB_TYPE(mp) == M_PROTO))
		return (-EAGAIN);	/* fast path for data */
	switch (DB_TYPE(mp)) {
	case M_DATA:
	case M_HPDATA:
	case M_PROTO:
	case M_PCPROTO:
	case M_CTL:
	case M_PCCTL:
	case M_SIG:
	case M_PCSIG:
	case M_RSE:
	case M_PCRSE:
		return (-EAGAIN);
	case M_FLUSH:
		return dl_m_flush(q, mp);
	case M_IOCTL:
		return dl_m_ioctl(q, mp);
	case M_IOCDATA:
		return dl_m_iocdata(q, mp);
	case M_READ:
		return dl_m_read(q, mp);
	default:
		return dl_m_other(q, mp);
	}
}
static fastcall noinline int
dl_msg_srv(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);

	if (unlikely(dl->proto.info.dl_primitive == 0))
		return (-EAGAIN);
	if (likely(DB_TYPE(mp) == M_DATA))
		return dl_m_data(q, mp);
	if (likely(DB_TYPE(mp) == M_PROTO))
		return dl_m_proto(q, mp);
	switch (DB_TYPE(mp)) {
	case M_DATA:
	case M_HPDATA:
		return dl_m_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return dl_m_proto(q, mp);
	case M_CTL:
	case M_PCCTL:
		return dl_m_ctl(q, mp);
	case M_SIG:
	case M_PCSIG:
		return dl_m_sig(q, mp);
	case M_RSE:
	case M_PCRSE:
		return dl_m_rse(q, mp);
	case M_FLUSH:
		return dl_m_flush(q, mp);
	case M_IOCTL:
		return dl_m_ioctl(q, mp);
	case M_IOCDATA:
		return dl_m_iocdata(q, mp);
	case M_READ:
		return dl_m_read(q, mp);
	default:
		return dl_m_other(q, mp);
	}
}

static fastcall noinline int
cd_msg_srv(queue_t *q, mblk_t *mp)
{
	if (likely(DB_TYPE(mp) == M_PROTO))
		return cd_m_proto(q, mp);
	switch (DB_TYPE(mp)) {
	case M_DATA:
	case M_HPDATA:
		return cd_m_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return cd_m_proto(q, mp);
	case M_SIG:
	case M_PCSIG:
		return cd_m_sig(q, mp);
	case M_CTL:
	case M_PCCTL:
		return cd_m_ctl(q, mp);
	case M_RSE:
	case M_PCRSE:
		return cd_m_rse(q, mp);
	case M_FLUSH:
		return cd_m_flush(q, mp);
	case M_ERROR:
		return cd_m_error(q, mp);
	case M_HANGUP:
	case M_UNHANGUP:
		return cd_m_hangup(q, mp);
	case M_SETOPTS:
	case M_PCSETOPTS:
		return cd_m_setopts(q, mp);
	case M_COPYIN:
	case M_COPYOUT:
		return cd_m_copy(q, mp);
	case M_IOCACK:
	case M_IOCNAK:
		return cd_m_iocack(q, mp);
	default:
		return cd_m_other(q, mp);
	}
}
static fastcall inline int
cd_msg_put(queue_t *q, mblk_t *mp)
{
	if (likely(DB_TYPE(mp) == M_PROTO))
		return (-EAGAIN);	/* fast path for data */
	switch (DB_TYPE(mp)) {
	case M_DATA:
	case M_HPDATA:
	case M_PROTO:
	case M_PCPROTO:
	case M_SIG:
	case M_PCSIG:
	case M_CTL:
	case M_PCCTL:
	case M_RSE:
	case M_PCRSE:
		return (-EAGAIN);	/* handle from service procedure */
	case M_FLUSH:
		return cd_m_flush(q, mp);
	case M_ERROR:
		return cd_m_error(q, mp);
	case M_HANGUP:
	case M_UNHANGUP:
		return cd_m_hangup(q, mp);
	case M_SETOPTS:
	case M_PCSETOPTS:
		return cd_m_setopts(q, mp);
	case M_COPYIN:
	case M_COPYOUT:
		return cd_m_copy(q, mp);
	case M_IOCACK:
	case M_IOCNAK:
		return cd_m_iocack(q, mp);
	default:
		return cd_m_other(q, mp);
	}
}

/*
 * --------------------------------------------------------------------------
 *
 * PUT AND SERVICE PROCEDURES
 *
 * --------------------------------------------------------------------------
 */

static streamscall int
dl_wput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flags & QSVCBUSY))) || dl_msg_put(q, mp))
		putq(q, mp);
	return (0);
}
static streamscall int
dl_wsrv(queue_t *q)
{
	caddr_t priv;

	if ((priv = mi_trylock(q)) != NULL) {
		mblk_t *mp;

		while ((mp = getq(q))) {
			if (dl_msg_srv(q, mp)) {
				putbq(q, mp);
				break;
			}
		}
		mi_unlock(priv);
	}
	return (0);
}

static streamscall int
cd_rsrv(queue_t *q)
{
	caddr_t priv;

	if ((priv = mi_trylock(q)) != NULL) {
		mblk_t *mp;

		while ((mp = getq(q))) {
			if (cd_msg_srv(q, mp)) {
				putbq(q, mp);
				break;
			}
		}
		mi_unlock(priv);
	}
	return (0);
}
static streamscall int
cd_rput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flags & QSVCBUSY))) || cd_msg_put(q, mp))
		putq(q, mp);
	return (0);
}

/*
 * --------------------------------------------------------------------------
 *
 * OPEN AND CLOSE ROUTINES
 *
 * --------------------------------------------------------------------------
 */

caddr_t lapb_opens = NULL;

static int
lapb_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	struct priv *p;
	struct dl *dl;
	struct cd *cd;
	dl_info_ack_t *dli;
	cd_info_ack_t *cdi;
	mblk_t *mp, *t_ack, *t_p, *t_rej, *t_busy;
	int err;
	pl_t pl;

	if (q->q_ptr != NULL)
		return (0);	/* already open */
	if (!(t_ack = mi_timer_alloc(q, sizeof(int)))) {
		return (ENOBUFS);
	}
	if (!(t_p = mi_timer_alloc(q, sizeof(int)))) {
		freeb(t_ack);
		return (ENOBUFS);
	}
	if (!(t_rej = mi_timer_alloc(q, sizeof(int)))) {
		freeb(t_ack);
		freeb(t_p);
		return (ENOBUFS);
	}
	if (!(t_busy = mi_timer_alloc(q, sizeof(int)))) {
		freeb(t_ack);
		freeb(t_p);
		freeb(t_rej);
		return (ENOBUFS);
	}
	if (!(mp = allocb(sizeof(cd_info_req_t), BPRI_WAITOK))) {
		freeb(t_ack);
		freeb(t_p);
		freeb(t_rej);
		freeb(t_busy);
		return (ENOBUFS);
	}
	if ((err = mi_open_comm(&lapb_opens, sizeof(*p), q, devp, oflags, sflag, crp))) {
		freeb(t_ack);
		freeb(t_p);
		freeb(t_rej);
		freeb(t_busy);
		freeb(mp);
		return (err);
	}

	p = PRIV(q);
	bzero(p, sizeof(*p));
	dl = &p->dlpi;
	cd = &p->cdi;

	/* initialize private structure */
	dl->priv = p;
	dl->cd = cd;
	dl->oq = q;
	dl->state = DL_UNATTACHED;
	dl->oldstate = DL_UNATTACHED;

	dl->t.ack = t_ack;
	dl->t.p = t_p;
	dl->t.rej = t_rej;
	dl->t.busy = t_busy;

	dli = &dl->proto.info;
	dli->dl_primitive = 0;	/* set to DL_INFO_ACK when initialized */
	dli->dl_max_sdu = 4096;
	dli->dl_min_sdu = 1;
	dli->dl_addr_length = 0;
	dli->dl_mac_type = DL_X25;
	dli->dl_reserved = 0;
	dli->dl_current_state = DL_UNATTACHED;
	dli->dl_sap_length = 1;
	dli->dl_service_mode = (DL_CODLS | DL_CLDLS);
	dli->dl_qos_length = sizeof(dl->proto.qos);
	dli->dl_qos_offset = sizeof(*dli);
	dli->dl_qos_range_length = sizeof(dl->proto.qor);
	dli->dl_qos_range_offset = dli->dl_qos_offset + dli->dl_qos_length;
	dli->dl_provider_style = DL_STYLE2;
	dli->dl_addr_offset = dli->dl_qos_range_offset + dli->dl_qos_range_length;
	dli->dl_version = DL_CURRENT_VERSION;
	dli->dl_brdcst_addr_length = 0;
	dli->dl_brdcst_addr_offset = dli->dl_addr_offset + dli->dl_addr_length;
	dli->dl_growth = 0;

	qos = &dl->proto.qos;
	qos->dl_qos_type = DL_QOS_CO_SEL1;
	qos->dl_rcv_throughput = DL_UNKNOWN;
	qos->dl_rcv_trans_delay = DL_UNKNOWN;
	qos->dl_xmt_throughput = DL_UNKNOWN;
	qos->dl_xmt_trans_delay = DL_UNKNOWN;
	qos->dl_priority = DL_UNKNOWN;
	qos->dl_protection = DL_UNKNOWN;
	qos->dl_residual_error = DL_UNKNOWN;
	qos->dl_resilience.dl_disc_prob = DL_UNKNOWN;
	qos->dl_resilience.dl_reset_prob = DL_UNKNOWN;

	qor = &dl->proto.qor;
	qor->dl_qos_type = DL_QOS_CO_RANGE1;
	qor->dl_rcv_throughput.dl_target_value = DL_UNKNOWN;
	qor->dl_rcv_throughput.dl_accept_value = DL_UNKNOWN;
	qor->dl_rcv_trans_delay.dl_target_value = DL_UNKNOWN;
	qor->dl_rcv_trans_delay.dl_accept_value = DL_UNKNOWN;
	qor->dl_xmt_throughput.dl_target_value = DL_UNKNOWN;
	qor->dl_xmt_throughput.dl_accept_value = DL_UNKNOWN;
	qor->dl_xmt_trans_delay.dl_target_value = DL_UNKNOWN;
	qor->dl_xmt_trans_delay.dl_accept_value = DL_UNKNOWN;
	qor->dl_priority.dl_min = DL_UNKNOWN;
	qor->dl_priority.dl_max = DL_UNKNOWN;
	qor->dl_protection.dl_min = DL_UNKNOWN;
	qor->dl_protection.dl_max = DL_UNKNOWN;
	qor->dl_residual_error = DL_UNKNOWN;
	qor->dl_resilience.dl_disc_prob = DL_UNKNOWN;
	qor->dl_resilience.dl_reset_prob = DL_UNKNOWN;

	dl->add_len = 0;
	dl->loc_len = 0;
	dl->rem_len = 0;

	cd->priv = p;
	cd->dl = dl;
	cd->oq = WR(q);
	cd->state = CD_UNUSABLE;
	cd->oldstate = CD_UNUSABLE;
	cd->style = CD_STYLE2;

	cdi = &cd->proto.info;
	cdi->cd_primitive = 0;	/* set to CD_INFO_ACK when initialized */
	cdi->cd_state = CD_UNUSABLE;
	cdi->cd_max_sdu = STRMAXPSZ;
	cdi->cd_min_sdu = STRMINPSZ;
	cdi->cd_class = CD_HDLC;
	cdi->cd_duplex = CD_FULLDUPLEX;
	cdi->cd_output_style = CD_UNACKEDOUTPUT;
	cdi->cd_features = (CD_CANREAD | CD_AUTOALLOW);
	cdi->cd_addr_length = 0;
	cdi->cd_ppa_style = CD_STYLE1;

	cd->sigs = 0;
	cd->alen = 0;

	/* Issue immediate info request. */
	DB_TYPE(mp) = M_PCPROTO;
	((cd_info_req_t *) mp->b_wptr)->cd_primitive = CD_INFO_REQ;
	mp->b_wptr += sizeof(cd_info_req_t);

	qprocson(q);
	putnext(q, mp);
	return (0);
}

static int
lapb_qclose(queue_t *q, int oflags, cred_t *crp)
{
	struct dl *dl = DL_PRIV(q);

	if (dl == NULL)
		return (0);
	qprocsoff(q);
	mi_timer_free(dl->t.ack);
	mi_timer_free(dl->t.p);
	mi_timer_free(dl->t.rej);
	mi_timer_free(dl->t.busy);
	mi_close_comm(&lapb_opens, q);
	return (0);
}

/*
 * --------------------------------------------------------------------------
 *
 * STREAMS INITIALIZATION
 *
 * --------------------------------------------------------------------------
 */

static struct qinit lapb_rinit = {
	.qi_putp = &cd_wput,
	.qi_srvp = &cd_wsrv,
	.qi_qopen = &lapb_qopen,
	.qi_qclose = &lapb_qclose,
	.qi_minfo = &lapb_minfo,
	.qi_mstat = &lapb_rstat,
};

static struct qinit lapb_winit = {
	.qi_putp = &dl_wput,
	.qi_srvp = &dl_wsrv,
	.qi_minfo = &lapb_minfo,
	.qi_mstat = &lapb_wstat,
};

struct streamtab lapb_info = {
	.st_rdinit = &lapb_rinit,
	.st_wrinit = &lapb_winit,
};

static modID_t modid = MOD_ID;

/*
 * --------------------------------------------------------------------------
 *
 * LINUX FAST STREAMS REGISTRATION
 *
 * --------------------------------------------------------------------------
 */

#ifdef LINUX

#ifndef module_param
MODULE_PARM(modid, "h");
#else				/* module_param */
module_param(modid, ushort, 0444);
#endif				/* module_param */
MODULE_PARM_DESC(modid, "Module ID for LAPB.  (0 for allocation.)");

#ifdef LIS
#define fmodsw _fmodsw
#endif				/* LIS */

static struct fmodsw lapb_fmod = {
	.f_name = MOD_NAME,
	.f_str = &lapb_info,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

static __init int
lapb_modinit(void)
{
	int err;

	cmn_err(CE_NOTE, MOD_BANNER);
	if ((err = register_strmod(&lapb_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not register module %d", MOD_NAME, (int) modid);
		return (err);
	}
	if (modid == 0)
		modid = err;
	return (0);
}

static __exit void
lapb_modexit(void)
{
	int err;

	if ((err = unregister_strmod(&lapb_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not unregister module, err = %d", MOD_NAME, err);
		return;
	}
	return;
}

module_init(lapb_modinit);
module_exit(lapb_modexit);

#endif				/* LINUX */
