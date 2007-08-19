/*****************************************************************************

 @(#) $RCSfile: tcap.c,v $ $Name:  $($Revision: 0.9.2.22 $) $Date: 2007/08/19 11:55:56 $

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

 Last Modified $Date: 2007/08/19 11:55:56 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: tcap.c,v $
 Revision 0.9.2.22  2007/08/19 11:55:56  brian
 - move stdbool.h, obviate need for YFLAGS, general workup

 Revision 0.9.2.21  2007/08/15 05:20:37  brian
 - GPLv3 updates

 Revision 0.9.2.20  2007/08/12 16:20:31  brian
 - new PPA handling

 Revision 0.9.2.19  2007/08/03 13:35:43  brian
 - manual updates, put ss7 modules in public release

 Revision 0.9.2.18  2007/07/14 01:35:15  brian
 - make license explicit, add documentation

 Revision 0.9.2.17  2007/03/25 19:00:26  brian
 - changes to support 2.6.20-1.2307.fc5 kernel

 Revision 0.9.2.16  2007/03/25 02:23:06  brian
 - add D_MP and D_MTPERQ flags

 Revision 0.9.2.15  2007/03/25 00:52:15  brian
 - synchronization updates

 Revision 0.9.2.14  2006/03/04 13:00:19  brian
 - FC4 x86_64 gcc 4.0.4 2.6.15 changes

 *****************************************************************************/

#ident "@(#) $RCSfile: tcap.c,v $ $Name:  $($Revision: 0.9.2.22 $) $Date: 2007/08/19 11:55:56 $"

static char const ident[] =
    "$RCSfile: tcap.c,v $ $Name:  $($Revision: 0.9.2.22 $) $Date: 2007/08/19 11:55:56 $ Copyright (c) 1997-2003 OpenSS7 Corporation.";

/*
 *  This is a TCAP (Transaction Capabilities Application Part) multiplexing
 *  driver whcih can have SCCP (Signalling Connection Control Part) or any
 *  other NPI conforming (e.g., X.25 NSP) stream I_LINK'ed or I_PLINK'ed
 *  underneath it to form a complete Transaction Capabilities Application Part
 *  protocol layer for SS7.
 */

#define _LFS_SOURCE	1
#define _SVR4_SOURCE	1
#define _MPS_SOURCE	1
#define _SUN_SOURCE	1

#include <sys/os7/compat.h>
#include <linux/socket.h>

#undef DB_TYPE
#define DB_TYPE(mp) (mp->b_datap->db_type)

#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/mtpi.h>
#include <ss7/mtpi_ioctl.h>

#include <sys/npi.h>
#include <sys/npi_ss7.h>
#include <sys/npi_mtp.h>
#include <sys/npi_sccp.h>
#include <ss7/sccpi.h>
#include <ss7/sccpi_ioctl.h>
//#include <sys/tpi.h>
//#include <sys/tpi_ss7.h>
//#include <sys/tpi_mtp.h>
//#include <sys/tpi_sccp.h>
//#include <sys/tpi_tr.h>
//#include <sys/tpi_tc.h>
#include <sys/tihdr.h>
#include <sys/xti.h>
#include <sys/xti_mtp.h>
#include <sys/xti_sccp.h>
#include <sys/xti_tcap.h>
//#include <sys/xti_tr.h>
//#include <sys/xti_tc.h>
#include <ss7/tcap.h>
#include <ss7/tcap_ioctl.h>

#define TCAP_DESCRIP	"SS7 TRANSACTION CAPABILITIES APPLICATION PART (TCAP) STREAMS MULTIPLEXING DRIVER."
#define TCAP_EXTRA	"Part of the OpenSS7 Stack for Linux Fast-STREAMS"
#define TCAP_REVISION	"OpenSS7 $RCSfile: tcap.c,v $ $Name:  $ ($Revision: 0.9.2.22 $) $Date: 2007/08/19 11:55:56 $"
#define TCAP_COPYRIGHT	"Copyright (c) 1997-2006 OpenSS7 Corporation.  All Rights Reserved."
#define TCAP_DEVICE	"Supports OpenSS7 SCCP NPI Interface Pseudo-Device Drivers."
#define TCAP_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define TCAP_LICENSE	"GPL"
#define TCAP_BANNER	TCAP_DESCRIP	"\n" \
			TCAP_EXTRA	"\n" \
			TCAP_REVISION	"\n" \
			TCAP_COPYRIGHT	"\n" \
			TCAP_DEVICE	"\n" \
			TCAP_CONTACT
#define TCAP_SPLASH	TCAP_DESCRIP	"\n" \
			TCAP_REVISION

#ifdef LINUX
MODULE_AUTHOR(TCAP_CONTACT);
MODULE_DESCRIPTION(TCAP_DESCRIP);
MODULE_SUPPORTED_DEVICE(TCAP_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(TCAP_LICENSE);
#endif				/* MODULE_LICENSE */
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-tcap");
#endif
#endif				/* LINUX */

#ifndef FIXME
#define FIXME -1
#endif

#ifdef LFS
#define TCAP_DRV_ID		CONFIG_STREAMS_TCAP_MODID
#define TCAP_DRV_NAME		CONFIG_STREAMS_TCAP_NAME
#define TCAP_CMAJORS		CONFIG_STREAMS_TCAP_NMAJORS
#define TCAP_CMAJOR_0		CONFIG_STREAMS_TCAP_MAJOR
#define TCAP_UNITS		CONFIG_STREAMS_TCAP_NMINORS
#endif

#ifndef TCAP_CMINOR_TRI
#define TCAP_CMINOR_TRI		1
#define TCAP_CMINOR_TCI		2
#define TCAP_CMINOR_TPI		3
#define TCAP_CMINOR_MGMT	4
#endif

#define TCAP_STYLE_TRI	TCAP_CMINOR_TRI
#define TCAP_STYLE_TCI	TCAP_CMINOR_TCI
#define TCAP_STYLE_TPI	TCAP_CMINOR_TPI
#define TCAP_STYLE_MGMT	TCAP_CMINOR_MGMT

/* 
 *  =========================================================================
 *
 *  STREAMS Definitions
 *
 *  =========================================================================
 */

#define DRV_ID		TCAP_DRV_ID
#define DRV_NAME	TCAP_DRV_NAME
#define CMAJORS		TCAP_CMAJORS
#define CMAJOR_0	TCAP_CMAJOR_0
#define UNITS		TCAP_UNITS
#ifdef MODULE
#define DRV_BANNER	TCAP_BANNER
#else				/* MODULE */
#define DRV_BANNER	TCAP_SPLASH
#endif				/* MODULE */

/* 
 *  =========================================================================
 *
 *  Logging
 *
 *  =========================================================================
 */

#define STRLOGNO	0	/* log always */
#define STRLOGIO	1	/* log Stream input-output controls */
#define STRLOGST	2	/* log Stream state transitions */
#define STRLOGTO	3	/* log Stream timeout */
#define STRLOGRX	4	/* log Stream received primitives */
#define STRLOGTX	5	/* log Stream issued primitives */
#define STRLOGTE	6	/* log Stream timer events */
#define STRLOGDA	7	/* log Stream data */

static inline const char *
msgname(uchar type)
{
	switch (type) {
	case M_DATA:
		return ("M_DATA");
	case M_PROTO:
		return ("M_PROTO");
	case M_BREAK:
		return ("M_BREAK");
	case M_PASSFP:
		return ("M_PASSFP");
#ifdef M_EVENT
	case M_EVENT:
		return ("M_EVENT");
#endif				/* M_EVENT */
	case M_SIG:
		return ("M_SIG");
	case M_DELAY:
		return ("M_DELAY");
	case M_CTL:
		return ("M_CTL");
	case M_IOCTL:
		return ("M_IOCTL");
	case M_SETOPTS:
		return ("M_SETOPTS");
	case M_RSE:
		return ("M_RSE");
#ifdef M_TRAIL
	case M_TRAIL:
		return ("M_TRAIL");
#endif				/* M_TRAIL */
#ifdef M_BACKWASH
	case M_BACKWASH:
		return ("M_BACKWASH");
#endif				/* M_BAKCWASH */
	case M_IOCACK:
		return ("M_IOCACK");
	case M_IOCNAK:
		return ("M_IOCNAK");
	case M_PCPROTO:
		return ("M_PCPROTO");
	case M_PCSIG:
		return ("M_PCSIG");
	case M_READ:
		return ("M_READ");
	case M_FLUSH:
		return ("M_FLUSH");
	case M_STOP:
		return ("M_STOP");
	case M_START:
		return ("M_START");
	case M_HANGUP:
		return ("M_HANGUP");
	case M_ERROR:
		return ("M_ERROR");
	case M_COPYIN:
		return ("M_COPYIN");
	case M_COPYOUT:
		return ("M_COPYOUT");
	case M_IOCDATA:
		return ("M_IOCDATA");
	case M_PCRSE:
		return ("M_PCRSE");
	case M_STOPI:
		return ("M_STOPI");
	case M_STARTI:
		return ("M_STARTI");
#ifdef M_PCCTL
	case M_PCCTL:
		return ("M_PCCTL");
#endif				/* M_PCCTL */
#ifdef M_PCSETOPTS
	case M_PCSETOPTS:
		return ("M_PCSETOPTS");
#endif				/* M_PCSETOPTS */
#ifdef M_PCEVENT
	case M_PCEVENT:
		return ("M_PCEVENT");
#endif				/* M_PCEVENT */
#ifdef M_UNHANGUP
	case M_UNHANGUP:
		return ("M_UNHANGUP");
#endif				/* M_UNHANGUP */
#ifdef M_NOTIFY
	case M_NOTIFY:
		return ("M_NOTIFY");
#endif				/* M_NOTIFY */
#ifdef M_HPDATA
	case M_HPDATA:
		return ("M_HPDATA");
#endif				/* M_HPDATA */
#ifdef M_LETSPLAY
	case M_LETSPLAY:
		return ("M_LETSPLAY");
#endif				/* M_LETSPLAY */
#ifdef M_DONTPLAY
	case M_DONTPLAY:
		return ("M_DONTPLAY");
#endif				/* M_DONTPLAY */
#ifdef M_BACKDONE
	case M_BACKDONE:
		return ("M_BACKDONE");
#endif				/* M_BACKDONE */
#ifdef M_PCTTY
	case M_PCTTY:
		return ("M_PCTTY");
#endif				/* M_PCTTY */
#ifdef M_CLOSE
	case M_CLOSE:
		return ("M_CLOSE");
#endif				/* M_CLOSE */
#ifdef M_CLOSE_REPL
	case M_CLOSE_REPL:
		return ("M_CLOSE_REPL");
#endif				/* M_CLOSE_REPL */
#ifdef M_MI
	case M_MI:
		return ("M_MI");
#endif				/* M_MI */
	default:
		return ("M_?????");
	}
}

static inline const char *
tc_iocname(int cmd)
{
	switch (_IOC_TYPE(cmd)) {
	case TCAP_IOC_MAGIC:
		switch (_IOC_NR(cmd)) {
		case TCAP_IOCGOPTION:
			return ("TCAP_IOCGOPTION");
		case TCAP_IOCSOPTION:
			return ("TCAP_IOCSOPTION");
		case TCAP_IOCGCONFIG:
			return ("TCAP_IOCGCONFIG");
		case TCAP_IOCSCONFIG:
			return ("TCAP_IOCSCONFIG");
		case TCAP_IOCTCONFIG:
			return ("TCAP_IOCTCONFIG");
		case TCAP_IOCCCONFIG:
			return ("TCAP_IOCCCONFIG");
		case TCAP_IOCGSTATEM:
			return ("TCAP_IOCGSTATEM");
		case TCAP_IOCCMRESET:
			return ("TCAP_IOCCMRESET");
		case TCAP_IOCGSTATSP:
			return ("TCAP_IOCGSTATSP");
		case TCAP_IOCSSTATSP:
			return ("TCAP_IOCSSTATSP");
		case TCAP_IOCGSTATS:
			return ("TCAP_IOCGSTATS");
		case TCAP_IOCCSTATS:
			return ("TCAP_IOCCSTATS");
		case TCAP_IOCGNOTIFY:
			return ("TCAP_IOCGNOTIFY");
		case TCAP_IOCSNOTIFY:
			return ("TCAP_IOCSNOTIFY");
		case TCAP_IOCCNOTIFY:
			return ("TCAP_IOCCNOTIFY");
		case TCAP_IOCCMGMT:
			return ("TCAP_IOCCMGMT");
		case TCAP_IOCCPASS:
			return ("TCAP_IOCCPASS");
		default:
			return ("(unknown number)");
		}
	default:
		return ("(unknown type)");
	}
}

static inline const char *
sc_iocname(int cmd)
{
	switch (_IOC_TYPE(cmd)) {
	case SCCP_IOC_MAGIC:
		switch (_IOC_NR(cmd)) {
		case SCCP_IOCGOPTION:
			return ("SCCP_IOCGOPTION");
		case SCCP_IOCSOPTION:
			return ("SCCP_IOCSOPTION");
		case SCCP_IOCGCONFIG:
			return ("SCCP_IOCGCONFIG");
		case SCCP_IOCSCONFIG:
			return ("SCCP_IOCSCONFIG");
		case SCCP_IOCTCONFIG:
			return ("SCCP_IOCTCONFIG");
		case SCCP_IOCCCONFIG:
			return ("SCCP_IOCCCONFIG");
		case SCCP_IOCGSTATEM:
			return ("SCCP_IOCGSTATEM");
		case SCCP_IOCCMRESET:
			return ("SCCP_IOCCMRESET");
		case SCCP_IOCGSTATSP:
			return ("SCCP_IOCGSTATSP");
		case SCCP_IOCSSTATSP:
			return ("SCCP_IOCSSTATSP");
		case SCCP_IOCGSTATS:
			return ("SCCP_IOCGSTATS");
		case SCCP_IOCCSTATS:
			return ("SCCP_IOCCSTATS");
		case SCCP_IOCGNOTIFY:
			return ("SCCP_IOCGNOTIFY");
		case SCCP_IOCSNOTIFY:
			return ("SCCP_IOCSNOTIFY");
		case SCCP_IOCCNOTIFY:
			return ("SCCP_IOCCNOTIFY");
		case SCCP_IOCCMGMT:
			return ("SCCP_IOCCMGMT");
		case SCCP_IOCCPASS:
			return ("SCCP_IOCCPASS");
		default:
			return ("(unknown number)");
		}
	default:
		return ("(unknown type)");
	}
}

static inline const char *
tc_primname(t_uscalar_t prim)
{
	switch (prim) {
	case TC_INFO_REQ:
		return ("TC_INFO_REQ");
	case TC_BIND_REQ:
		return ("TC_BIND_REQ");
	case TC_UNBIND_REQ:
		return ("TC_UNBIND_REQ");
	case TC_SUBS_BIND_REQ:
		return ("TC_SUBS_BIND_REQ");
	case TC_SUBS_UNBIND_REQ:
		return ("TC_SUBS_UNBIND_REQ");
	case TC_OPTMGMT_REQ:
		return ("TC_OPTMGMT_REQ");
	case TC_UNI_REQ:
		return ("TC_UNI_REQ");
	case TC_BEGIN_REQ:
		return ("TC_BEGIN_REQ");
	case TC_BEGIN_RES:
		return ("TC_BEGIN_RES");
	case TC_CONT_REQ:
		return ("TC_CONT_REQ");
	case TC_END_REQ:
		return ("TC_END_REQ");
	case TC_ABORT_REQ:
		return ("TC_ABORT_REQ");
	case TC_INFO_ACK:
		return ("TC_INFO_ACK");
	case TC_BIND_ACK:
		return ("TC_BIND_ACK");
	case TC_SUBS_BIND_ACK:
		return ("TC_SUBS_BIND_ACK");
	case TC_OK_ACK:
		return ("TC_OK_ACK");
	case TC_ERROR_ACK:
		return ("TC_ERROR_ACK");
	case TC_OPTMGMT_ACK:
		return ("TC_OPTMGMT_ACK");
	case TC_UNI_IND:
		return ("TC_UNI_IND");
	case TC_BEGIN_IND:
		return ("TC_BEGIN_IND");
	case TC_BEGIN_CON:
		return ("TC_BEGIN_CON");
	case TC_CONT_IND:
		return ("TC_CONT_IND");
	case TC_END_IND:
		return ("TC_END_IND");
	case TC_ABORT_IND:
		return ("TC_ABORT_IND");
	case TC_NOTICE_IND:
		return ("TC_NOTICE_IND");
	case TC_INVOKE_REQ:
		return ("TC_INVOKE_REQ");
	case TC_RESULT_REQ:
		return ("TC_RESULT_REQ");
	case TC_ERROR_REQ:
		return ("TC_ERROR_REQ");
	case TC_CANCEL_REQ:
		return ("TC_CANCEL_REQ");
	case TC_REJECT_REQ:
		return ("TC_REJECT_REQ");
	case TC_INVOKE_IND:
		return ("TC_INVOKE_IND");
	case TC_RESULT_IND:
		return ("TC_RESULT_IND");
	case TC_ERROR_IND:
		return ("TC_ERROR_IND");
	case TC_CANCEL_IND:
		return ("TC_CANCEL_IND");
	case TC_REJECT_IND:
		return ("TC_REJECT_IND");
	default:
		return ("(unknown)");
	}
}

static inline const char *
tr_primname(t_uscalar_t prim)
{
	switch (prim) {
	case TR_INFO_REQ:
		return ("TR_INFO_REQ");
	case TR_BIND_REQ:
		return ("TR_BIND_REQ");
	case TR_UNBIND_REQ:
		return ("TR_UNBIND_REQ");
	case TR_OPTMGMT_REQ:
		return ("TR_OPTMGMT_REQ");
	case TR_UNI_REQ:
		return ("TR_UNI_REQ");
	case TR_BEGIN_REQ:
		return ("TR_BEGIN_REQ");
	case TR_BEGIN_RES:
		return ("TR_BEGIN_RES");
	case TR_CONT_REQ:
		return ("TR_CONT_REQ");
	case TR_END_REQ:
		return ("TR_END_REQ");
	case TR_ABORT_REQ:
		return ("TR_ABORT_REQ");
	case TR_ADDR_REQ:
		return ("TR_ADDR_REQ");
	case TR_INFO_ACK:
		return ("TR_INFO_ACK");
	case TR_BIND_ACK:
		return ("TR_BIND_ACK");
	case TR_OK_ACK:
		return ("TR_OK_ACK");
	case TR_ERROR_ACK:
		return ("TR_ERROR_ACK");
	case TR_OPTMGMT_ACK:
		return ("TR_OPTMGMT_ACK");
	case TR_UNI_IND:
		return ("TR_UNI_IND");
	case TR_BEGIN_IND:
		return ("TR_BEGIN_IND");
	case TR_BEGIN_CON:
		return ("TR_BEGIN_CON");
	case TR_CONT_IND:
		return ("TR_CONT_IND");
	case TR_END_IND:
		return ("TR_END_IND");
	case TR_ABORT_IND:
		return ("TR_ABORT_IND");
	case TR_NOTICE_IND:
		return ("TR_NOTICE_IND");
	case TR_ADDR_ACK:
		return ("TR_ADDR_ACK");
	default:
		return ("(unknown)");
	}
}

static inline const char *
sc_primname(np_ulong prim)
{
	switch (prim) {
	case N_CONN_REQ:
		return ("N_CONN_REQ");
	case N_CONN_RES:
		return ("N_CONN_RES");
	case N_DISCON_REQ:
		return ("N_DISCON_REQ");
	case N_DATA_REQ:
		return ("N_DATA_REQ");
	case N_EXDATA_REQ:
		return ("N_EXDATA_REQ");
	case N_INFO_REQ:
		return ("N_INFO_REQ");
	case N_BIND_REQ:
		return ("N_BIND_REQ");
	case N_UNBIND_REQ:
		return ("N_UNBIND_REQ");
	case N_UNITDATA_REQ:
		return ("N_UNITDATA_REQ");
	case N_OPTMGMT_REQ:
		return ("N_OPTMGMT_REQ");
	case N_CONN_IND:
		return ("N_CONN_IND");
	case N_CONN_CON:
		return ("N_CONN_CON");
	case N_DISCON_IND:
		return ("N_DISCON_IND");
	case N_DATA_IND:
		return ("N_DATA_IND");
	case N_EXDATA_IND:
		return ("N_EXDATA_IND");
	case N_INFO_ACK:
		return ("N_INFO_ACK");
	case N_BIND_ACK:
		return ("N_BIND_ACK");
	case N_ERROR_ACK:
		return ("N_ERROR_ACK");
	case N_OK_ACK:
		return ("N_OK_ACK");
	case N_UNITDATA_IND:
		return ("N_UNITDATA_IND");
	case N_UDERROR_IND:
		return ("N_UDERROR_IND");
	case N_DATACK_REQ:
		return ("N_DATACK_REQ");
	case N_DATACK_IND:
		return ("N_DATACK_IND");
	case N_RESET_REQ:
		return ("N_RESET_REQ");
	case N_RESET_IND:
		return ("N_RESET_IND");
	case N_RESET_RES:
		return ("N_RESET_RES");
	case N_RESET_CON:
		return ("N_RESET_CON");
	case N_NOTICE_IND:
		return ("N_NOTICE_IND");
	case N_INFORM_REQ:
		return ("N_INFORM_REQ");
	case N_INFORM_IND:
		return ("N_INFORM_IND");
	case N_COORD_REQ:
		return ("N_COORD_REQ");
	case N_COORD_RES:
		return ("N_COORD_RES");
	case N_COORD_IND:
		return ("N_COORD_IND");
	case N_COORD_CON:
		return ("N_COORD_CON");
	case N_STATE_REQ:
		return ("N_STATE_REQ");
	case N_STATE_IND:
		return ("N_STATE_IND");
	case N_PCSTATE_IND:
		return ("N_PCSTATE_IND");
	case N_TRAFFIC_IND:
		return ("N_TRAFFIC_IND");
	default:
		return ("(unknown)");
	}
}

/* 
 *  =========================================================================
 *
 *  Private Structures
 *
 *  =========================================================================
 */

#define TCAP_BUF_MAXLEN	    32
typedef struct tcap_buf {
	uchar buf[TCAP_BUF_MAXLEN];
	size_t len;
} tcap_buf_t;

typedef struct tcap_options {
	tcap_buf_t ctx;			/* application context */
	tcap_buf_t inf;			/* user information */
	t_uscalar_t pcl;		/* protocol class */
	t_uscalar_t ret;		/* return option */
	t_uscalar_t imp;		/* importance */
	t_uscalar_t seq;		/* sequence control */
	t_uscalar_t pri;		/* priority */
	t_uscalar_t sls;		/* signalling link selection */
	t_uscalar_t mp;			/* message priority */
	t_uscalar_t mtu;		/* maximum transfer unit */
} tcap_options_t;

struct tcap_options opt_defaults;

struct df;				/* DF structure */
struct tc;				/* TC structure - TCAP user - upper multiplex stream */
struct iv;				/* IV structure - (dynamic) invoke */
struct op;				/* OP structure - operation */
struct dg;				/* DG structure - (dynamic) dialog */
struct ac;				/* AC structure - application context */
struct tr;				/* TR structure - (dynamic) transaction */
struct te;				/* TE structure - TCAP entity */
struct sp;				/* SP structure - SCCP-SAP */
struct sc;				/* SC structure - SCCP provider - lower multiplex stream */

/*
 *  Trasnaction Capabilities Application Part
 *  -----------------------------------------------------------
 *  The TCAP structure (TC) represents an open TC User, TR User, or TP User Stream.  Which interface
 *  style is applied to the Stream depends upon the major and minor device number opened.  TCAP User
 *  Streams can be associated with a TCAP Entity, either by the major and minor device number
 *  opened, or by temporarily linking SCCP provider Streams under the open Stream as a control
 *  Stream, or by binding the Stream.
 */
typedef struct tc {
	STR_DECLARATION (struct tc);	/* stream declaration */
	dev_t dev;
	struct {
		struct tc *next;
		struct tc **prev;
		struct te *te;		/* Associated TCAP Entity */
	} te;
	struct ac *ac;			/* Bound Application Context */
	SLIST_HEAD (tr, tr);		/* transaction list */
	SLIST_HEAD (dg, dg);		/* dialogue list */
	t_uscalar_t token;		/* token for this stream */
	t_uscalar_t conind;		/* max connection inds */
	t_uscalar_t outcnt;		/* out connection inds */
	t_uscalar_t pclass;		/* protocol class */
	t_uscalar_t reterr;		/* return on error */
	struct sccp_addr src;		/* bound src address */
	uint8_t saddr[SCCP_MAX_ADDR_LENGTH];
	struct sccp_addr dst;		/* bound dst address */
	uint8_t daddr[SCCP_MAX_ADDR_LENGTH];
	bufq_t conq;			/* connection indication queue */
	struct tcap_options options;	/* settable options */
	struct lmi_option option;	/* TCAP protocol variant and options */
	struct tcap_notify_tc notify;	/* TCAP notifications */
	struct tcap_timers_tc timers;	/* TCAP timers */
	struct tcap_statem_tc statem;	/* TCAP state machine */
	struct tcap_opt_conf_tc config;	/* TCAP configuration */
	struct tcap_stats_tc statsp;	/* TCAP statistics periods */
	struct tcap_stats_tc stamp;	/* TCAP statistics timestamps */
	struct tcap_stats_tc stats;	/* TCAP statistics */
} tcap_t;

#define TC_PRIV(__q) ((struct tc *)(__q)->q_ptr)

static caddr_t tcap_opens = NULL;

static struct tc *lm_ctrl = NULL;

static struct tc *tcap_alloc_priv(queue_t *, struct tc **, dev_t *, cred_t *, minor_t);
static struct tc *tc_get(struct tc *);
static struct tc *tc_lookup(uint);
static uint tc_get_id(uint);
static void tcap_free_priv(struct tc *);
static void tc_put(struct tc *);
static struct tc *tcap_open_alloc(queue_t *, dev_t *, cred_t *, minor_t);
static void tcap_close_free(struct tc *);

static struct tc *tc_acquire(queue_t *);
static void tc_release(struct tc *);

/*
 *  Invoke
 *  -----------------------------------------------------------
 *  The Invoke (IV) structure is a dynamic structure that represents an instance of an operation
 *  within a dialog.  Invokes are created when invoke components are encountered within the dialog
 *  and are destroyed when a response arrives or the invocation is otherwise aborted.  Invokes are
 *  hashed against dialogs by their invoke id.  Each invoke can be associated with an operation
 *  structure by its operation code.
 */
typedef struct iv {
	HEAD_DECLARATION (struct iv);	/* head declaration */
	SLIST_LINKAGE (dg, iv, dg);	/* associated dialogue */
	SLIST_LINKAGE (op, iv, op);	/* associated operation */
	t_uscalar_t oper;		/* operation */
	t_uscalar_t iid;		/* invoke id */
	t_uscalar_t lid;		/* linked id */
	struct tcap_notify_iv notify;	/* invoke notifications */
	struct tcap_timers_iv timers;	/* invoke protocol timers */
	struct tcap_statem_iv statem;	/* invoke state machine */
	struct tcap_opt_conf_iv config;	/* invoke configuration */
	struct tcap_stats_iv statsp;	/* invoke statistics periods */
	struct tcap_stats_iv stamp;	/* invoke statsistics timestamps */
	struct tcap_stats_iv stats;	/* invoke statistics */
} iv_t;

static struct iv *tcap_alloc_iv(uint);
static struct iv *iv_get(struct iv *);
static struct iv *iv_lookup(struct dg *, uint);
static uint iv_get_id(uint);
static void tcap_free_iv(struct iv *);
static void iv_put(struct iv *);

/*
 *  Operation
 *  -----------------------------------------------------------
 *  The Operation (OP) structure is an optional static configuration structure that describes a
 *  valid operation within an Application Context and provides additional information pertinent to
 *  that operation (such as response timeout value).
 */
typedef struct op {
	HEAD_DECLARATION (struct op);	/* head declaration */
	SLIST_LINKAGE (ac, op, ac);	/* associated application context */
	SLIST_HEAD (iv, iv);		/* invokes for this operation */
	t_uscalar_t oper;		/* operation */
	struct tcap_notify_op notify;	/* operation notifications */
	struct tcap_timers_op timers;	/* operation protocol timers */
	struct tcap_statem_op statem;	/* operation state machine */
	struct tcap_opt_conf_op config;	/* operation configuration */
	struct tcap_stats_op statsp;	/* operation statistics periods */
	struct tcap_stats_op stamp;	/* operation statistics timestamps */
	struct tcap_stats_op stats;	/* operation statistics */
} op_t;

static struct op *tcap_alloc_op(uint);
static struct op *op_get(struct op *);
static struct op *op_lookup(struct ac *, uint);
static uint op_get_id(uint);
static void tcap_free_op(struct op *);
static void op_put(struct op *);

/*
 *  Dialog
 *  -----------------------------------------------------------
 *  The Dialog (DG) structure is a dynamic structure that represents a dialog and maintains the
 *  dialog state.  Dialogs are created when transactions are created for the TCI interface style and
 *  TPI interface style, but not for the TRI interface.  For the TPI interface style, each dialog is
 *  associated with a TP User Stream.  For the TCI interface style, dialogs are hashed against the
 *  TC User Stream by dialog id.
 */
typedef struct dg {
	HEAD_DECLARATION (struct dg);	/* head declaration */
	struct tc *tc;			/* associated TCAP User Stream */
	SLIST_LINKAGE (tr, dg, tr);	/* associated transaction */
	SLIST_LINKAGE (ac, dg, ac);	/* associated application context */
	SLIST_LINKAGE (iv, dg, iv);	/* list of invocations */
	t_uscalar_t did;		/* dialogue id */
	struct tcap_notify_dg notify;	/* dialog notifications */
	struct tcap_timers_dg timers;	/* dialog protocol timers */
	struct tcap_statem_dg statem;	/* dialog state machine */
	struct tcap_opt_conf_dg config;	/* dialog configuration */
	struct tcap_stats_dg statsp;	/* dialog statistics periods */
	struct tcap_stats_dg stamp;	/* dialog statsistics timestamps */
	struct tcap_stats_dg stats;	/* dialog statistics */
} dg_t;

static struct dg *tcap_alloc_dg(uint);
static struct dg *dg_get(struct dg *);
static struct dg *dg_lookup(struct tc *, uint);
static uint dg_get_id(uint);
static void tcap_free_dg(struct dg *);
static void dg_put(struct dg *);

/*
 *  Application Context
 *  -----------------------------------------------------------
 *  The Application Context (AC) structure has the primary purpose of providing a location where AC
 *  information can be maintained separate from a structure that is associated with an open TC User
 *  Stream.  This is for information that should persist beyond the unbinding and rebinding of TC
 *  User Streams.  When the TC User Stream is bound, it is associated one-to-one with an AC
 *  structure.  AC structures can be created on demand with default information when a TC User
 *  stream is bound to a not yet existing AC.
 *
 *  An application context has an Application Context OID or Universal Integer associated with it
 *  that identifies the Application Context to which it corresponds.  It could also represent a
 *  wildcard Application Context.  The Application Context defines the range of dialogues that are
 *  serviced by the associated TC User for the TCAP Entity with which the AC is associated.  Each
 *  Application Context can have a defined set of valid Operations, or can have a null list of
 *  operations permitting any operation within the Application Context.
 */
typedef struct ac {
	HEAD_DECLARATION (struct ac);	/* head declaration */
	struct tc *tc;			/* bound TC user */
	SLIST_HEAD (dg, dg);		/* list of dialogs */
	SLIST_HEAD (op, op);		/* list of operations */
	SLIST_LINKAGE (te, ac, te);	/* associated TCAP Entity */
	t_uscalar_t acid;		/* application context id */
	struct tcap_notify_ac notify;	/* application context notifications */
	struct tcap_timers_ac timers;	/* application context protocol timers */
	struct tcap_statem_ac statem;	/* application context state machine */
	struct tcap_opt_conf_ac config;	/* application context configuration */
	struct tcap_stats_ac statsp;	/* application context statistics periods */
	struct tcap_stats_ac stamp;	/* application context statsistics timestamps */
	struct tcap_stats_ac stats;	/* application context statistics */
} ac_t;

static struct ac *tcap_alloc_ac(uint);
static struct ac *ac_get(struct ac *);
static struct ac *ac_lookup(struct tc *, uint);
static uint ac_get_id(uint);
static void tcap_free_ac(struct ac *);
static void ac_put(struct ac *);

/*
 *  Transaction
 *  -----------------------------------------------------------
 */
typedef struct tr {
	HEAD_DECLARATION (struct tr);	/* head declaration */
	SLIST_LINKAGE (tc, tr, tc);	/* associated TCAP user */
	SLIST_LINKAGE (te, tr, te);	/* associated TCAP entity */
	SLIST_HEAD (iv, iv);		/* invokes for this transaction */
	t_uscalar_t cid;		/* correlation id */
	t_uscalar_t tid;		/* transaction id */
	t_uscalar_t ocls;		/* operation class */
	struct tcap_notify_tr notify;	/* transaction notifications */
	struct tcap_timers_tr timers;	/* transaction protocol timers */
	struct tcap_statem_tr statem;	/* transaction state machine */
	struct tcap_opt_conf_tr config;	/* transaction configuration */
	struct tcap_stats_tr statsp;	/* transaction statistics periods */
	struct tcap_stats_tr stamp;	/* transaction statsistics timestamps */
	struct tcap_stats_tr stats;	/* transaction statistics */
} tr_t;

static struct tr *tcap_alloc_tr(struct tc *, uint, uint);
static struct tr *tr_get(struct tr *);
static struct tr *tr_lookup_cid(struct tc *, uint);
static struct tr *tr_lookup_tid(struct tc *, uint);
static uint tr_get_id(uint);
static void tcap_free_tr(struct tr *);
static void tr_put(struct tr *);

/*
 *  TCAP Entity
 *  -----------------------------------------------------------
 *  The TCAP entity structure (TE) is the primary structure that identifies a TCAP system.  A TCAP
 *  system can contain a collection of SCCP SAPs which represent the specific entry and exit points
 *  for SCCP messages to an from the SS7 network.  Each TE structure can have multiple SP structures
 *  representing SCCP SAPs beneath it providing connectivity to the SS7 network via SC structures in
 *  turn (SC structures representing linked lower SCCP Streams).  Each TE structure can serve
 *  multiple AC structures representing one or more Application Contexts, which in turn serve TC
 *  structures (TC structures representing open upper TCAP Streams).
 *
 *  The TCAP Entity contains the primary lock for each TCAP Entity system.  The primary lock is used
 *  to protect the total state of the TCAP Entity and all its subtending components.  The structure
 *  of the TCAP Entity (linkage between structures within the TCAP Entity system) is protected by
 *  the master read-write TCAP multiplexing driver lock.
 */
typedef struct te {
	HEAD_DECLARATION (struct te);	/* head declaration */
	struct {
		spinlock_t lock;
		int users;
	} sq;
	struct {
		struct tc *lm;
	} lm;
	uint teid;			/* TCAP Entity id */
	SLIST_HEAD (tc, tc);		/* TC Users bound to this TE */
	SLIST_HEAD (ac, ac);		/* Application Context list */
	SLIST_HEAD (sp, sp);		/* SCCP-SAP list */
	SLIST_HEAD (sc, sc);		/* SCCP linked under this TE */
	struct tcap_notify_te notify;	/* TCAP Entity notifications */
	struct tcap_timers_te timers;	/* TCAP Entity protocol timers */
	struct tcap_statem_te statem;	/* TCAP state machine */
	struct tcap_opt_conf_te config;	/* TCAP Entity configuration */
	struct tcap_stats_te statsp;	/* TCAP Entity statistics periods */
	struct tcap_stats_te stamp;	/* TCAP Entity statistics timestamps */
	struct tcap_stats_te stats;	/* TCAP Entity statistics */
} te_t;

static struct te *tcap_alloc_te(struct tc *, uint);
static struct te *te_get(struct te *);
static struct te *te_lookup(struct tc *, uint);
static uint te_get_id(uint);
static void tcap_free_te(struct te *);
static void te_put(struct te *);
static int te_trylock(struct te *, queue_t *);
static void te_unlock(struct te *);

/*
 *  SCCP SAP
 *  -----------------------------------------------------------
 *  The primary purpose the SCCP SAP structure is to provide a location where SCCP SAP information
 *  can be maintained separate from a structure that is associated with a linked SCCP Stream.  This
 *  is for information that should persist beyond the unlinking and relinking of SCCP Streams
 *  beneath the multiplexing driver.  When an SCCP Stream is linked under the multiplexing driver,
 *  it awaits configuration.  When the SCCP Stream is configured, it is associated one-to-one with
 *  an SCCP SAP structure.  SCCP SAP structures can be created on demand with default information
 *  when an SCCP Stream is associated with a not yet existing SCCP SAP.
 */
typedef struct sp {
	HEAD_DECLARATION (struct sp);	/* head declaration */
	struct {
		struct te *te;		/* SCCP-SAP TCAP Entity */
	} te;
	struct {
		struct sc *sc;		/* SCCP-SAP SCCP Stream */
	} sc;
	struct tcap_notify_sp notify;	/* SCCP-SAP notifications */
	struct tcap_timers_sp timers;	/* SCCP-SAP protocol timers */
	struct tcap_statem_sp statem;	/* SCCP-SAP state machine */
	struct tcap_opt_conf_sp config;	/* SCCP-SAP configuration */
	struct tcap_stats_sp statsp;	/* SCCP-SAP statistics periods */
	struct tcap_stats_sp stamp;	/* SCCP-SAP statistics timestamps */
	struct tcap_stats_sp stats;	/* SCCP-SAP statistics */
} sp_t;

static struct sp *tcap_alloc_sp(uint);
static struct sp *sp_get(struct sp *);
static struct sp *sp_lookup(struct tc *, uint);
static uint sp_get_id(uint);
static void tcap_free_sp(struct sp *);
static void sp_put(struct sp *);

/*
 *  SCCP
 *  -----------------------------------------------------------
 */
typedef struct sc {
	STR_DECLARATION (struct sc);	/* stream declaration */
	struct {
		struct sc *next;
		struct sc **prev;
		struct te *te;
	} te;
	struct {
		struct sp *sp;		/* SCCP-SAP instance for this stream */
	} sp;
	size_t outcnt;			/* outstanding connection indications */
	t_uscalar_t psdu;		/* pSDU size */
	t_uscalar_t pidu;		/* pIDU size */
	t_uscalar_t podu;		/* pODU size */
	struct sccp_addr add;		/* local address */
	uint8_t addr[SCCP_MAX_ADDR_LENGTH];
	struct sccp_addr rem;		/* remote address */
	uint8_t raddr[SCCP_MAX_ADDR_LENGTH];
	t_uscalar_t pcl;
	t_uscalar_t ret;
	t_uscalar_t imp;
	t_uscalar_t seq;
	t_uscalar_t sls;
	t_uscalar_t mp;
	N_info_ack_t prot;
	struct lmi_option option;	/* protocol variant and options */
	struct tcap_notify_sc notify;	/* TCAP notifications */
	struct tcap_timers_sc timers;	/* TCAP protocol timers */
	struct tcap_statem_sc statem;	/* TCAP state machine */
	struct tcap_opt_conf_sc config;	/* TCAP configuration */
	struct tcap_stats_sc statsp;	/* TCAP statistics periods */
	struct tcap_stats_sc stamp;	/* TCAP statistics timestamp */
	struct tcap_stats_sc stats;	/* TCAP statistics */
} sccp_t;

#define SC_PRIV(__q) ((struct sc *)(__q)->q_ptr)

static caddr_t tcap_links = NULL;

static struct sc *sccp_alloc_link(queue_t *, int, cred_t *, struct te *);
static struct sc *sc_get(struct sc *);
static struct sc *sc_lookup(uint);
static uint sc_get_id(uint);
static void sccp_free_unlink(struct sc *);
static void sc_put(struct sc *);

static struct sc *sc_acquire(queue_t *);
static void sc_release(struct sc *);

/*
 *  Default
 *  -----------------------------------------------------------
 */
typedef struct df {
	spinlock_t lock;
	SLIST_HEAD (tc, tc);		/* master list of TCAP users */
	SLIST_HEAD (tr, tr);		/* master list of transactions */
	SLIST_HEAD (sp, sp);		/* master list of SCCP-SAPs */
	SLIST_HEAD (sc, sc);		/* master list of SCCP providers */
	struct lmi_option option;	/* default protocol variant and options */
	struct tcap_notify_df notify;	/* default notifications */
	struct tcap_timers_df timers;	/* default protocol timers */
	struct tcap_statem_df statem;	/* default state machine */
	struct tcap_opt_conf_df config;	/* default configuration */
	struct tcap_stats_df statsp;	/* default statistics periods */
	struct tcap_stats_df stamp;	/* default statistics timestamps */
	struct tcap_stats_df stats;	/* default statistics */
} df_t;
static struct df master;

static inline struct df *
df_lookup(struct tc *tc, uint id)
{
	if (id != 0)
		return (NULL);
	return (&master);
}

/*
 *  =========================================================================
 *
 *  Specialized Multiplex Locking
 *
 *  =========================================================================
 */
static int
te_trylock(struct te *te, queue_t *q)
{
	psw_t psw;
	int rtn = 0;

	spin_lock_irqsave(&te->sq.lock, psw);
	if (te->sq.users == 0) {
		te->sq.users++;
		rtn = 1;
	}
	spin_unlock_irqrestore(&te->sq.lock, psw);
	return (rtn);

}
static void
te_unlock(struct te *te)
{
	psw_t psw;

	spin_lock_irqsave(&te->sq.lock, psw);
	te->sq.users = 0;
	spin_unlock_irqrestore(&te->sq.lock, psw);
}

static struct tc *
tc_acquire(queue_t *q)
{
	/* FIXME: lock te if available too */
	return ((struct tc *) mi_trylock(q));
}

static void
tc_release(struct tc *tc)
{
	/* FIXME: release te if available too */
	mi_unlock((caddr_t) tc);
}

static struct sc *
sc_acquire(queue_t *q)
{
	/* FIXME: lock te if available too */
	return ((struct sc *) mi_trylock(q));
}

static void
sc_release(struct sc *sc)
{
	/* FIXME: release te if available too */
	mi_unlock((caddr_t) sc);
}

/*
 *  =========================================================================
 *
 *  TCAP Message Structures
 *
 *  =========================================================================
 */

/* Message Types */
#define	TCAP_MT_UNI		1	/* Unidirectional */
#define	TCAP_MT_QWP		2	/* Query w/ permission */
#define	TCAP_MT_QWOP		3	/* Query w/o permission */
#define	TCAP_MT_CWP		4	/* Conversation w/ permission */
#define	TCAP_MT_CWOP		5	/* Conversation w/o permission */
#define	TCAP_MT_RESP		6	/* Response */
#define	TCAP_MT_ABORT		7	/* Abort */

/* Component Types */
#define	TCAP_CT_INVOKE_L	1	/* Invoke (Last) */
#define	TCAP_CT_INVOKE_NL	2	/* Invoke (Not Last) */
#define	TCAP_CT_RESULT_L	3	/* Return Result (Last) */
#define	TCAP_CT_RESULT_NL	4	/* Return Result (Not Last) */
#define	TCAP_CT_REJECT		5	/* Reject */
#define	TCAP_CT_ERROR		6	/* Error */

#define TCAP_TAG_UNIV_INT	2	/* UNIV Integer */
#define TCAP_TAG_UNIV_OSTR	4	/* UNIV Octet String */
#define TCAP_TAG_UNIV_OID	6	/* UNIV Object Id */
#define	TCAP_TAG_UNIV_PSEQ	16	/* UNIV Parameter Sequence */
#define TCAP_TAG_UNIV_UTC	17	/* UNIV Universal Time */
#define TCAP_TAG_UNIV_SEQ	48	/* UNIV Sequence */

#define	TCAP_TAG_TCAP_ACG	 1	/* TCAP ACG Indicators */
#define	TCAP_TAG_TCAP_STA	 2	/* TCAP Standard Announcement */
#define TCAP_TAG_TCAP_CUA	 3	/* TCAP Customized Announcment */
#define	TCAP_TAG_TCAP_TDIG	 4	/* TCAP Digits */
#define	TCAP_TAG_TCAP_SUEC	 5	/* TCAP Standard User Error Code */
#define	TCAP_TAG_TCAP_PDTA	 6	/* TCAP Problem Data */
#define	TCAP_TAG_TCAP_TCGPA	 7	/* TCAP SCCP Calling Party Address */
#define	TCAP_TAG_TCAP_TRSID	 8	/* TCAP Transaction ID */
#define	TCAP_TAG_TCAP_PCTY	 9	/* TCAP Package Type */
#define	TCAP_TAG_TCAP_SKEY	10	/* TCAP Service Key (Constructor) */
#define	TCAP_TAG_TCAP_BISTAT	11	/* TCAP Busy/Idle Status */
#define	TCAP_TAG_TCAP_CFSTAT	12	/* TCAP Call Forwarding Status */
#define	TCAP_TAG_TCAP_ORIGR	13	/* TCAP Origination Restrictions */
#define	TCAP_TAG_TCAP_TERMR	14	/* TCAP Terminating Restrictions */
#define	TCAP_TAG_TCAP_DNMAP	15	/* TCAP DN to Line Service TYpe Mapping */
#define	TCAP_TAG_TCAP_DURTN	16	/* TCAP Duration */
#define	TCAP_TAG_TCAP_RETD	17	/* TCAP Return Data (Constructor) */
#define	TCAP_TAG_TCAP_BCRQ	18	/* TCAP Bearer Capability Requested */
#define	TCAP_TAG_TCAP_BCSUP	19	/* TCAP Bearer Capability Supported */
#define	TCAP_TAG_TCAP_REFID	20	/* TCAP Reference Id */
#define	TCAP_TAG_TCAP_BGROUP	21	/* TCAP Business Group */
#define	TCAP_TAG_TCAP_SNI	22	/* TCAP Signalling Networks Identifier */
#define	TCAP_TAG_TCAP_MWIT	23	/* TCAP Message Waiting Indicator Type */

#define	TCAP_TAG_PRIV_UNI	 1	/* ANSI Unidirectional */
#define	TCAP_TAG_PRIV_QWP	 2	/* ANSI Query w/ permission */
#define	TCAP_TAG_PRIV_QWOP	 3	/* ANSI Query w/o permission */
#define	TCAP_TAG_PRIV_RESP	 4	/* ANSI Response */
#define	TCAP_TAG_PRIV_CWP	 5	/* ANSI Conversaion w/ permission */
#define	TCAP_TAG_PRIV_CWOP	 6	/* ANSI Conversaion w/o permission */
#define	TCAP_TAG_PRIV_TRSID	 7	/* ANSI Transaction Id */
#define	TCAP_TAG_PRIV_CSEQ	 8	/* ANSI Component Sequence */
#define	TCAP_TAG_PRIV_INKL	 9	/* ANSI Invoke (Last) */
#define	TCAP_TAG_PRIV_RRL	10	/* ANSI Return Result (Last) */
#define	TCAP_TAG_PRIV_RER	11	/* ANSI Return Error */
#define	TCAP_TAG_PRIV_REJ	12	/* ANSI Reject */
#define	TCAP_TAG_PRIV_INK	13	/* ANSI Invoke (Not Last) */
#define	TCAP_TAG_PRIV_RR	14	/* ANSI Result (Not Last) */
#define	TCAP_TAG_PRIV_CORID	15	/* ANSI Correlation Id(s) */
#define	TCAP_TAG_PRIV_NOPCO	16	/* ANSI National Operation Code */
#define	TCAP_TAG_PRIV_POPCO	17	/* ANSI Private Operation Code */
#define	TCAP_TAG_PRIV_PSET	18	/* ANSI Parameter Set */
#define TCAP_TAG_PRIV_NECODE	19	/* ANSI National Error Code */
#define TCAP_TAG_PRIV_PECODE	20	/* ANSI Private Error Code */
#define	TCAP_TAG_PRIV_PBCODE	21	/* ANSI Reject Problem Code */
#define	TCAP_TAG_PRIV_PSEQ	21	/* ANSI Parameter Sequence */
#define	TCAP_TAG_PRIV_ABORT	22	/* ANSI Abort */
#define	TCAP_TAG_PRIV_PCAUSE	23	/* ANSI P-Abort Cause */
#define	TCAP_TAG_PRIV_U_ABORT	24	/* ANSI User Abort Information */
#define	TCAP_TAG_PRIV_DLGP	25	/* ANSI Dialog Portion */
#define	TCAP_TAG_PRIV_VERSION	26	/* ANSI Protocol Version */
#define	TCAP_TAG_PRIV_CONTEXT	27	/* ANSI Integer Application Context */
#define	TCAP_TAG_PRIV_CTX_OID	28	/* ANSI OID Application Context */
#define	TCAP_TAG_PRIV_UINFO	29	/* ANSI User Information */

#define TCAP_TAG_APPL_UNI	1	/* ITUT Unidirectional */
#define TCAP_TAG_APPL_BEGIN	2	/* ITUT Begin Transaction */
#define TCAP_TAG_APPL_END	4	/* ITUT End Transaction */
#define TCAP_TAG_APPL_CONT	5	/* ITUT Continue Transaction */
#define TCAP_TAG_APPL_ABORT	7	/* ITUT Abort Transaction */
#define TCAP_TAG_APPL_ORIGID	8	/* ITUT Origination Transaction Id */
#define TCAP_TAG_APPL_DESTID	9	/* ITUT Destination Transaction Id */
#define TCAP_TAG_APPL_PCAUSE	10	/* ITUT P-Abort Cause */
#define TCAP_TAG_APPL_DLGP	11	/* ITUT Dialog Portion */
#define TCAP_TAG_APPL_CSEQ	12	/* ITUT Component Portion */

#define TCAP_TAG_APPL_AUDT_PDU	0	/* ITUT AUDT APDU */
#define TCAP_TAG_APPL_AARQ_PDU	0	/* ITUT AARQ APDU */
#define TCAP_TAG_APPL_AARE_PDU	1	/* ITUT AARE APDU */
#define TCAP_TAG_APPL_RLRQ_PDU	2	/* ITUT RLRQ APDU */
#define TCAP_TAG_APPL_RLRE_PDU	3	/* ITUT RLRE APDU */
#define TCAP_TAG_APPL_ABRT_PDU	4	/* ITUT ABRT APDU */

#define TCAP_TAG_CNTX_LID	0	/* Linked Id */
#define TCAP_TAG_CNTX_INK	1	/* Invoke */
#define TCAP_TAG_CNTX_RRL	2	/* Return Result (Last) */
#define TCAP_TAG_CNTX_RER	3	/* Return Error */
#define TCAP_TAG_CNTX_REJ	4	/* Reject */
#define TCAP_TAG_CNTX_RR	7	/* Return Result (Not Last) */

typedef struct sc_msg {
	struct sccp_addr orig;		/* Originating address */
	uchar oaddr[SCCP_MAX_ADDR_LENGTH];	/* Originating address signals */
	struct sccp_addr dest;		/* Destination address */
	uchar daddr[SCCP_MAX_ADDR_LENGTH];	/* Destination address signals */
	t_uscalar_t pcl;		/* protocol class */
	t_uscalar_t imp;		/* importance */
	t_uscalar_t seq;		/* sequence control */
	t_uscalar_t ret;		/* return option */
	t_uscalar_t sls;		/* signalling link selection */
	t_uscalar_t mp;			/* message priority */
} sc_msg_t;

typedef struct tr_msg {
	struct sc_msg sc;		/* SCCP message */
	t_uscalar_t type;		/* Message type */
	t_uscalar_t parms;		/* Bit mask of parms included */
	t_uscalar_t origid;		/* Originating Transaction Id */
	t_uscalar_t destid;		/* Destination Transaction Id */
	t_uscalar_t cause;		/* Abort Cause */
	t_uscalar_t version;		/* TCAP version flags */
	uchar *dlg_beg;			/* beg of dialog portion */
	uchar *dlg_end;			/* end of dialog portion */
	uchar *cmp_beg;			/* beg of components or u-abort info */
	uchar *cmp_end;			/* end of components or u-abort info */
	uchar *abt_beg;			/* beg of u-abort info */
	uchar *abt_end;			/* end of u-abort info */
} tr_msg_t;

#define TCAP_PTF_ORIGID	(1<<0)	/* Originating Transaction id */
#define TCAP_PTF_DESTID	(1<<1)	/* Destination Transaction id */
#define TCAP_PTF_DLGP	(1<<2)	/* Dialog Portion */
#define TCAP_PTF_CSEQ	(1<<3)	/* Component Portion */

typedef struct tc_msg {
	t_uscalar_t type;		/* Component type */
	t_uscalar_t parms;		/* Bit mask of parms included */
	t_uscalar_t iid;		/* Invoke Id */
	t_uscalar_t lid;		/* Linked Id */
	t_uscalar_t opcode;		/* Operation Code */
	t_uscalar_t ecode;		/* Error Code */
	t_uscalar_t pcode;		/* Problem Code */
	uchar *prm_beg;			/* beg of parameters */
	uchar *prm_end;			/* end of parameters */
} tc_msg_t;

#define TCAP_PTF_IID	(1<<0)	/* Invoke Id */
#define TCAP_PTF_LID	(1<<1)	/* Linked Id */
#define TCAP_PTF_NOPCO	(1<<2)	/* National Opcode */
#define TCAP_PTF_POPCO	(1<<3)	/* Private Opcode */
#define TCAP_PTF_LOPCO	(1<<4)	/* Local Opcode */
#define TCAP_PTF_PSEQ	(1<<5)	/* Parameter Sequence */
#define TCAP_PTF_NECODE	(1<<6)	/* National Error Code */
#define TCAP_PTF_PECODE	(1<<7)	/* Private Error Code */
#define TCAP_PTF_LECODE	(1<<8)	/* Local Error Code */
#define TCAP_PTF_PCODE	(1<<9)	/* Problem Code */

/*
 *  =========================================================================
 *
 *  TCAP DECODE Message Functions
 *
 *  =========================================================================
 */
/*
 *  General purpose decoding functions.
 *  -------------------------------------------------------------------------
 */
static inline int
unpack_taglen(uchar **p, uchar **e, t_uscalar_t *tag, t_uscalar_t *cls)
{
	t_uscalar_t len;

	if (*p >= *e)
		return (-EMSGSIZE);
	*cls = *(*p)++;
	if ((*cls & 0x1f) != 0x1f) {	/* tag is not extended */
		*tag = (*cls & 0x1f);
	} else {		/* tag is extended */
		uint8_t ptag;

		*tag = 0;
		if (*p >= *e)
			return (-EMSGSIZE);
		ptag = *(*p)++;
		*tag |= (ptag & 0x7f);
		if (ptag & 0x80) {
			if (*p >= *e)
				return (-EMSGSIZE);
			*tag <<= 7;
			ptag = *(*p)++;
			*tag |= (ptag & 0x7f);
			if (ptag & 0x80) {
				if (*p >= *e)
					return (-EMSGSIZE);
				*tag <<= 7;
				ptag = *(*p)++;
				*tag |= (ptag & 0x7f);
				if (ptag & 0x80) {
					if (*p >= *e)
						return (-EMSGSIZE);
					*tag <<= 7;
					ptag = *(*p)++;
					*tag |= (ptag & 0x7f);
					if (ptag & 0x80)
						return (-EMSGSIZE);
				}
			}
		}
	}
	if (*p >= *e)
		return (-EMSGSIZE);
	len = *(*p)++;
	if ((len & 0x80) != 0x00) {	/* extended length */
		t_uscalar_t plen = len & 0x7f;

		if (plen > 4 || plen == 0)
			return (-EMSGSIZE);
		len = 0;
		while (plen--) {
			if (*p >= *e)
				return (-EMSGSIZE);
			len |= *(*p)++;
			len <<= 8;
		}
		if (*p + len > *e)
			return (-EMSGSIZE);
		*e = *p + len;
	}
	*cls &= ~0x1f;
	return (0);
}

/* unpack universal primitive class 0 form 0 */
static inline int
unpack_univ_prim(uchar **p, uchar **e, t_uscalar_t *tag)
{
	int err;
	t_uscalar_t cls;

	if ((err = unpack_taglen(p, e, tag, &cls)))
		return (err);
	if ((cls & 0xe0) == 0x00)
		return (0);
	return (-EINVAL);
}

/* unpack universal constructor class 0 form 1 */
static inline int
unpack_univ_cons(uchar **p, uchar **e, t_uscalar_t *tag)
{
	int err;
	t_uscalar_t cls;

	if ((err = unpack_taglen(p, e, tag, &cls)))
		return (err);
	if ((cls & 0xe0) == 0x20)
		return (0);
	return (-EINVAL);
}

/* unpack application-wide primitive class 1 form 0 */
static inline int
unpack_appl_prim(uchar **p, uchar **e, t_uscalar_t *tag)
{
	int err;
	t_uscalar_t cls;

	if ((err = unpack_taglen(p, e, tag, &cls)))
		return (err);
	if ((cls & 0xe0) == 0x80)
		return (0);
	return (-EINVAL);
}

/* unpack application-wide constructor class 1 form 1 */
static inline int
unpack_appl_cons(uchar **p, uchar **e, t_uscalar_t *tag)
{
	int err;
	t_uscalar_t cls;

	if ((err = unpack_taglen(p, e, tag, &cls)))
		return (err);
	if ((cls & 0xe0) == 0xa0)
		return (0);
	return (-EINVAL);
}

/* unpack context-specific primitive class 2 form 0 */
static inline int
unpack_ctxt_prim(uchar **p, uchar **e, t_uscalar_t *tag)
{
	int err;
	t_uscalar_t cls;

	if ((err = unpack_taglen(p, e, tag, &cls)))
		return (err);
	if ((cls & 0xe0) == 0x40)
		return (0);
	return (-EINVAL);
}

/* unpack context-specific constructor class 2 form 1 */
static inline int
unpack_ctxt_cons(uchar **p, uchar **e, t_uscalar_t *tag)
{
	int err;
	t_uscalar_t cls;

	if ((err = unpack_taglen(p, e, tag, &cls)))
		return (err);
	if ((cls & 0xe0) == 0x60)
		return (0);
	return (-EINVAL);
}

/* unpack private primitive class 3 form 0 */
static inline int
unpack_priv_prim(uchar **p, uchar **e, t_uscalar_t *tag)
{
	int err;
	t_uscalar_t cls;

	if ((err = unpack_taglen(p, e, tag, &cls)))
		return (err);
	if ((cls & 0xe0) == 0xc0)
		return (0);
	return (-EINVAL);
}

/* unpack private constructor class 3 form 1 */
static inline int
unpack_priv_cons(uchar **p, uchar **e, t_uscalar_t *tag)
{
	int err;
	t_uscalar_t cls;

	if ((err = unpack_taglen(p, e, tag, &cls)))
		return (err);
	if ((cls & 0xe0) == 0xe0)
		return (0);
	return (-EINVAL);
}
static inline int
unpack_implicit_int(uchar **p, uchar *e, t_uscalar_t *val)
{
	/* FIXME */
	return (-EFAULT);
}
static inline int
unpack_explicit_int(uchar **p, uchar *e, t_uscalar_t *val)
{
	/* FIXME */
	return (-EFAULT);
}

/*
 *  =========================================================================
 *
 *  TCAP DECODE Messages (Component (TC) Sub-Layer)
 *
 *  =========================================================================
 *
 *  ANSI PRIVATE-TCAP version of Components.
 *
 *  -------------------------------------------------------------------------
 *
 *  UNPACK ANSI Correlation Ids.
 *  ------------------------------------------------------------------------
 */
static inline int
unpack_priv_corids(uchar **p, uchar *e, struct tc_msg *m, const t_uscalar_t ctype)
{
	int err;
	t_uscalar_t tag;

	if ((err = unpack_priv_prim(p, &e, &tag)))
		return (err);
	if (tag != TCAP_TAG_PRIV_CORID)
		return (-EINVAL);
	switch (e - *p) {
	case 0:		/* no ids */
		switch (ctype) {
		case TCAP_CT_INVOKE_L:
		case TCAP_CT_INVOKE_NL:
		case TCAP_CT_REJECT:
			return (0);
		}
		return (-EPROTO);
	case 1:		/* invoke id */
		switch (ctype) {
		default:
		case TCAP_CT_INVOKE_L:
		case TCAP_CT_INVOKE_NL:
		case TCAP_CT_RESULT_L:
		case TCAP_CT_RESULT_NL:
		case TCAP_CT_REJECT:
		case TCAP_CT_ERROR:
			if (*p >= e)
				return (-EMSGSIZE);
			m->iid = *(*p)++;
			m->parms |= TCAP_PTF_IID;
			break;
		}
		return (-EPROTO);
	case 2:		/* invoke id and correlation id */
		switch (ctype) {
		case TCAP_CT_INVOKE_L:
		case TCAP_CT_INVOKE_NL:
			if (*p >= e)
				return (-EMSGSIZE);
			m->iid = *(*p)++;
			m->parms |= TCAP_PTF_IID;
			if (*p >= e)
				return (-EMSGSIZE);
			m->lid = *(*p)++;
			m->parms |= TCAP_PTF_LID;
			return (0);
		}
		return (-EPROTO);
	}
	return (-EINVAL);
}

/*
 *  UNPACK ANSI Opcodes
 *  ------------------------------------------------------------------------
 */
static inline int
unpack_priv_opcode(uchar **p, uchar *e, struct tc_msg *m)
{
	int err;
	t_uscalar_t tag;

	if ((err = unpack_priv_prim(p, &e, &tag)))
		return (err);
	if (e - *p != 2)
		return (-EMSGSIZE);
	switch (tag) {
	case TCAP_TAG_PRIV_NOPCO:
		if ((err = unpack_implicit_int(p, e, &m->opcode)))
			return (err);
		m->parms |= TCAP_PTF_NOPCO;
		return (0);
	case TCAP_TAG_PRIV_POPCO:
		if ((err = unpack_implicit_int(p, e, &m->opcode)))
			return (err);
		m->parms |= TCAP_PTF_POPCO;
		return (0);
	}
	return (-EPROTO);
}

/*
 *  UNPACK ANSI Error Codes
 *  ------------------------------------------------------------------------
 */
static inline int
unpack_priv_ecode(uchar **p, uchar *e, struct tc_msg *m)
{
	int err;
	t_uscalar_t tag;

	if ((err = unpack_priv_prim(p, &e, &tag)))
		return (err);
	switch (tag) {
	case TCAP_TAG_PRIV_NECODE:
		if ((err = unpack_explicit_int(p, e, &m->ecode)))
			return (err);
		m->parms |= TCAP_PTF_NECODE;
		return (0);
	case TCAP_TAG_PRIV_PECODE:
		if ((err = unpack_explicit_int(p, e, &m->ecode)))
			return (err);
		m->parms |= TCAP_PTF_PECODE;
		return (0);
	}
	return (-EPROTO);
}

/*
 *  UNPACK ANSI Problem Codes
 *  ------------------------------------------------------------------------
 */
static inline int
unpack_priv_pcode(uchar **p, uchar *e, struct tc_msg *m)
{
	int err;
	t_uscalar_t tag;

	if ((err = unpack_priv_prim(p, &e, &tag)))
		return (err);
	if ((err = unpack_implicit_int(p, e, &m->pcode)))
		return (err);
	m->parms |= TCAP_PTF_PCODE;
	return (0);
}

/*
 *  UNPACK ANSI Component Parameter Sets
 *  ------------------------------------------------------------------------
 */
static inline int
unpack_priv_pset(uchar **p, uchar *e, struct tc_msg *m)
{
	int err;
	t_uscalar_t tag;

	if ((err = unpack_priv_cons(p, &e, &tag)))
		return (err);
	if (tag != TCAP_TAG_PRIV_PSET && tag != TCAP_TAG_PRIV_PSEQ)
		return (-EINVAL);
	m->prm_beg = *p;
	m->prm_end = e;
	*p = e;
	m->parms |= TCAP_PTF_PSEQ;
	return (0);
}

/*
 *  ANSI INVOKE (LAST)
 *  ------------------------------------------------------------------------
 */
static int
tcap_priv_dec_inkl(uchar **p, uchar *e, struct tc_msg *m)
{
	int err;

	m->type = TCAP_CT_INVOKE_L;
	if ((err = unpack_priv_corids(p, e, m, TCAP_CT_INVOKE_L)))
		return (err);
	if ((err = unpack_priv_opcode(p, e, m)))
		return (err);
	if ((err = unpack_priv_pset(p, e, m)))
		return (err);
	if (*p != e)
		return (-EMSGSIZE);
	return (0);
}

/*
 *  ANSI INVOKE (NOT LAST)
 *  ------------------------------------------------------------------------
 */
static int
tcap_priv_dec_ink(uchar **p, uchar *e, struct tc_msg *m)
{
	int err;

	m->type = TCAP_CT_INVOKE_NL;
	if ((err = unpack_priv_corids(p, e, m, TCAP_CT_INVOKE_NL)))
		return (err);
	if ((err = unpack_priv_opcode(p, e, m)))
		return (err);
	if ((err = unpack_priv_pset(p, e, m)))
		return (err);
	if (*p != e)
		return (-EMSGSIZE);
	return (0);
}

/*
 *  ANSI RETURN RESULT (LAST)
 *  ------------------------------------------------------------------------
 */
static int
tcap_priv_dec_rrl(uchar **p, uchar *e, struct tc_msg *m)
{
	int err;

	m->type = TCAP_CT_RESULT_L;
	if ((err = unpack_priv_corids(p, e, m, TCAP_CT_RESULT_L)))
		return (err);
	if ((err = unpack_priv_pset(p, e, m)))
		return (err);
	if (*p != e)
		return (-EMSGSIZE);
	return (0);
}

/*
 *  ANSI RETURN RESULT (NOT LAST)
 *  ------------------------------------------------------------------------
 */
static int
tcap_priv_dec_rr(uchar **p, uchar *e, struct tc_msg *m)
{
	int err;

	m->type = TCAP_CT_RESULT_NL;
	if ((err = unpack_priv_corids(p, e, m, TCAP_CT_RESULT_NL)))
		return (err);
	if ((err = unpack_priv_pset(p, e, m)))
		return (err);
	if (*p != e)
		return (-EMSGSIZE);
	return (0);
}

/*
 *  ANSI RETURN ERROR
 *  ------------------------------------------------------------------------
 */
static int
tcap_priv_dec_rer(uchar **p, uchar *e, struct tc_msg *m)
{
	int err;

	m->type = TCAP_CT_ERROR;
	if ((err = unpack_priv_corids(p, e, m, TCAP_CT_ERROR)))
		return (err);
	if ((err = unpack_priv_ecode(p, e, m)))
		return (err);
	if ((err = unpack_priv_pset(p, e, m)))
		return (err);
	if (*p != e)
		return (-EMSGSIZE);
	return (0);
}

/*
 *  ANSI REJECT
 *  ------------------------------------------------------------------------
 */
static int
tcap_priv_dec_rej(uchar **p, uchar *e, struct tc_msg *m)
{
	int err;

	m->type = TCAP_CT_REJECT;
	if ((err = unpack_priv_corids(p, e, m, TCAP_CT_REJECT)))
		return (err);
	if ((err = unpack_priv_pcode(p, e, m)))
		return (err);
	if ((err = unpack_priv_pset(p, e, m)))
		return (err);
	if (*p != e)
		return (-EMSGSIZE);
	return (0);
}

/*
 *  ANSI Component Decoder
 *  ------------------------------------------------------------------------
 */
static inline int
tcap_priv_dec_comp(uchar **p, uchar *e, struct tc_msg *m)
{
	int err;
	t_uscalar_t tag;

	if (!(err = unpack_appl_cons(p, &e, &tag)))
		switch (tag) {
		case TCAP_TAG_PRIV_INKL:	/* Invoke (Last) */
			if (!(err = tcap_priv_dec_inkl(p, e, m)))
				return (0);
			break;
		case TCAP_TAG_PRIV_INK:	/* Invoke (Not Last) */
			if (!(err = tcap_priv_dec_ink(p, e, m)))
				return (0);
			break;
		case TCAP_TAG_PRIV_RRL:	/* Return Result (Last) */
			if (!(err = tcap_priv_dec_rrl(p, e, m)))
				return (0);
			break;
		case TCAP_TAG_PRIV_RR:	/* Return Result (Not Last) */
			if (!(err = tcap_priv_dec_rr(p, e, m)))
				return (0);
			break;
		case TCAP_TAG_PRIV_RER:	/* Return Error */
			if (!(err = tcap_priv_dec_rer(p, e, m)))
				return (0);
			break;
		case TCAP_TAG_PRIV_REJ:	/* Reject */
			if (!(err = tcap_priv_dec_rej(p, e, m)))
				return (0);
			break;
		default:
			err = -EINVAL;
			break;
		}
	return (err);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  ITUT APPLICATION-TCAP version of Components.
 *
 *  -------------------------------------------------------------------------
 *
 *  UNPACK ITUT Correlation Ids.
 *  ------------------------------------------------------------------------
 */
static inline int
unpack_univ_iid(uchar **p, uchar *e, struct tc_msg *m)
{
	(void) p;
	(void) e;
	(void) m;
	/* FIXME */
	return (-EFAULT);
}
static inline int
unpack_univ_lid(uchar **p, uchar *e, struct tc_msg *m)
{
	(void) p;
	(void) e;
	(void) m;
	/* FIXME */
	return (-EFAULT);
}
static inline int
unpack_univ_corids(uchar **p, uchar *e, struct tc_msg *m, const t_uscalar_t ctype)
{
	int err;

	switch (ctype) {
	case TCAP_CT_INVOKE_L:
		if ((err = unpack_univ_iid(p, e, m)))
			return (err);
		m->parms |= TCAP_PTF_IID;
		if ((err = unpack_univ_lid(p, e, m)))	/* optional */
			return (err);
		m->parms |= TCAP_PTF_LID;
		return (0);
	case TCAP_CT_RESULT_L:
	case TCAP_CT_RESULT_NL:
	case TCAP_CT_ERROR:
		if ((err = unpack_univ_iid(p, e, m)))
			return (err);
		m->parms |= TCAP_PTF_IID;
		return (0);
	case TCAP_CT_REJECT:
		/* could be NULL */
		if ((err = unpack_univ_iid(p, e, m)))
			return (err);
		m->parms |= TCAP_PTF_IID;
		return (0);
	}
	return (-EFAULT);
}

/*
 *  UNPACK ITUT Opcodes
 *  ------------------------------------------------------------------------
 */
static inline int
unpack_univ_opcode(uchar **p, uchar *e, struct tc_msg *m)
{
	int err;
	t_uscalar_t tag;

	if ((err = unpack_univ_prim(p, &e, &tag)))
		return (err);
	if (tag == TCAP_TAG_UNIV_OID)	/* 6 */
		return (-EOPNOTSUPP);	/* we don't support OID opcodes */
	if (tag != TCAP_TAG_UNIV_INT)	/* 2 */
		return (-EPROTO);
	if ((err = unpack_implicit_int(p, e, &m->opcode)))
		return (err);
	m->parms |= TCAP_PTF_LOPCO;
	return (0);
}

/*
 *  UNPACK ITUT Parameter Sequences
 *  ------------------------------------------------------------------------
 */
static inline int
unpack_univ_pseq(uchar **p, uchar *e, struct tc_msg *m, const t_uscalar_t opt)
{
	int err;
	t_uscalar_t tag;

	if (opt) {
		if (*p >= e)
			return (0);
		if (**p != (0x20 | TCAP_TAG_UNIV_SEQ))
			return (0);
	}
	if ((err = unpack_univ_cons(p, &e, &tag)))
		return (err);
	if (tag != TCAP_TAG_UNIV_SEQ)
		return (-EINVAL);
	m->prm_beg = *p;
	m->prm_end = e;
	*p = e;
	m->parms |= TCAP_PTF_PSEQ;
	return (0);
}

/*
 *  UNPACK ITUT Result Opcode and Parameter Sequences
 *  ------------------------------------------------------------------------
 */
static inline int
unpack_univ_rseq(uchar **p, uchar *e, struct tc_msg *m)
{
	int err;
	t_uscalar_t tag;

	if (*p >= e)
		return (0);
	if (**p != (0x20 | TCAP_TAG_UNIV_SEQ))
		return (0);
	if ((err = unpack_univ_cons(p, &e, &tag)))
		return (err);
	if (tag != TCAP_TAG_UNIV_SEQ)
		return (-EINVAL);
	if ((err = unpack_univ_opcode(p, e, m)))
		return (err);
	if ((err = unpack_univ_pseq(p, e, m, 0)))
		return (err);
	return (0);
}

/*
 *  UNPACK ITUT Error Codes
 *  ------------------------------------------------------------------------
 */
static inline int
unpack_univ_ecode(uchar **p, uchar *e, struct tc_msg *m)
{
	int err;
	t_uscalar_t tag;

	if ((err = unpack_univ_prim(p, &e, &tag)))
		return (err);
	switch (tag) {
	case TCAP_TAG_UNIV_INT:
		if ((err = unpack_implicit_int(p, e, &m->ecode)))
			return (err);
		m->parms |= TCAP_PTF_LECODE;
		return (0);
	case TCAP_TAG_UNIV_OID:
		return (-EOPNOTSUPP);	/* don't support global error codes */
	}
	return (-EPROTO);
}

/*
 *  UNPACK ITUT Problem Codes
 *  ------------------------------------------------------------------------
 */
static inline int
unpack_ctxt_pcode(uchar **p, uchar *e, struct tc_msg *m)
{
	int err;
	t_uscalar_t tag;
	t_uscalar_t ptype;

	if ((err = unpack_ctxt_prim(p, &e, &tag)))
		return (err);
	ptype = tag;
	if ((err = unpack_implicit_int(p, e, &m->pcode)))
		return (err);
	m->pcode |= ptype << 16;
	m->parms |= TCAP_PTF_PCODE;
	return (0);
}

/*
 *  ITUT INVOKE (LAST)
 *  ------------------------------------------------------------------------
 */
static int
tcap_ctxt_dec_inkl(uchar **p, uchar *e, struct tc_msg *m)
{
	int err;

	m->type = TCAP_CT_INVOKE_L;
	if ((err = unpack_univ_corids(p, e, m, TCAP_CT_INVOKE_L)))
		return (err);
	if ((err = unpack_univ_opcode(p, e, m)))
		return (err);
	if ((err = unpack_univ_pseq(p, e, m, 1)))
		return (err);
	if (*p != e)
		return (-EMSGSIZE);
	return (0);
}

/*
 *  ITUT RETURN RESULT (LAST)
 *  ------------------------------------------------------------------------
 */
static int
tcap_ctxt_dec_rrl(uchar **p, uchar *e, struct tc_msg *m)
{
	int err;

	m->type = TCAP_CT_RESULT_L;
	if ((err = unpack_univ_corids(p, e, m, TCAP_CT_RESULT_L)))
		return (err);
	if ((err = unpack_univ_rseq(p, e, m)))
		return (err);
	if (*p != e)
		return (-EMSGSIZE);
	return (0);
}

/*
 *  ITUT RETURN RESULT (NOT LAST)
 *  ------------------------------------------------------------------------
 */
static int
tcap_ctxt_dec_rr(uchar **p, uchar *e, struct tc_msg *m)
{
	int err;

	m->type = TCAP_CT_RESULT_NL;
	if ((err = unpack_univ_corids(p, e, m, TCAP_CT_RESULT_NL)))
		return (err);
	if ((err = unpack_univ_rseq(p, e, m)))
		return (err);
	if (*p != e)
		return (-EMSGSIZE);
	return (0);
}

/*
 *  ITUT RETURN ERROR
 *  ------------------------------------------------------------------------
 */
static int
tcap_ctxt_dec_rer(uchar **p, uchar *e, struct tc_msg *m)
{
	int err;

	m->type = TCAP_CT_ERROR;
	if ((err = unpack_univ_corids(p, e, m, TCAP_CT_ERROR)))
		return (err);
	if ((err = unpack_univ_ecode(p, e, m)))
		return (err);
	if ((err = unpack_univ_pseq(p, e, m, 1)))
		return (err);
	if (*p != e)
		return (-EMSGSIZE);
	return (0);
}

/*
 *  ITUT REJECT
 *  ------------------------------------------------------------------------
 */
static int
tcap_ctxt_dec_rej(uchar **p, uchar *e, struct tc_msg *m)
{
	int err;

	m->type = TCAP_CT_REJECT;
	if ((err = unpack_univ_corids(p, e, m, TCAP_CT_REJECT)))
		return (err);
	if ((err = unpack_ctxt_pcode(p, e, m)))
		return (err);
	if (*p != e)
		return (-EMSGSIZE);
	return (0);
}

/*
 *  ITUT Component Decoder
 *  ------------------------------------------------------------------------
 */
static inline int
tcap_appl_dec_comp(uchar **p, uchar *e, struct tc_msg *m)
{
	int err;
	t_uscalar_t tag;

	if (!(err = unpack_ctxt_cons(p, &e, &tag)))
		switch (tag) {
		case TCAP_TAG_CNTX_INK:	/* Invoke */
			if (!(err = tcap_ctxt_dec_inkl(p, e, m)))
				return (0);
			break;
		case TCAP_TAG_CNTX_RRL:	/* Return Result (Last) */
			if (!(err = tcap_ctxt_dec_rrl(p, e, m)))
				return (0);
			break;
		case TCAP_TAG_CNTX_RER:	/* Return Error */
			if (!(err = tcap_ctxt_dec_rer(p, e, m)))
				return (0);
			break;
		case TCAP_TAG_CNTX_REJ:	/* Reject */
			if (!(err = tcap_ctxt_dec_rej(p, e, m)))
				return (0);
			break;
		case TCAP_TAG_CNTX_RR:	/* Return Result (Not Last) */
			if (!(err = tcap_ctxt_dec_rr(p, e, m)))
				return (0);
			break;
		default:
			err = -EINVAL;
			break;
		}
	return (err);
}

/*
 *  =========================================================================
 *
 *  TCAP DECODE Messages (Transaction (TR) Sub-Layer)
 *
 *  =========================================================================
 */
/*
 *  PRIVATE-TCAP version of Packages.  (Used by the TR sub-layer.)
 *  -------------------------------------------------------------------------
 */
static inline int
unpack_priv_trsid(uchar **p, uchar *e, struct tr_msg *m, const t_uscalar_t mtype)
{
	int err;
	t_uscalar_t tag;

	if ((err = unpack_priv_prim(p, &e, &tag)))
		return (err);
	if (tag != TCAP_TAG_PRIV_TRSID)
		return (-EINVAL);
	switch (e - *p) {
	case 0:		/* no transaction ids */
		switch (mtype) {
		case TCAP_MT_UNI:
			return (0);
		}
		return (-EPROTO);
	case 4:		/* orig or dest transaction id */
		switch (mtype) {
		case TCAP_MT_QWP:
		case TCAP_MT_QWOP:
		case TCAP_MT_RESP:
		case TCAP_MT_ABORT:
			unpack_implicit_int(p, e, &m->origid);
			m->parms |= TCAP_PTF_ORIGID;
			return (0);
		}
		return (-EPROTO);
	case 8:		/* orig and dest transaction id */
		switch (mtype) {
		case TCAP_MT_CWP:
		case TCAP_MT_CWOP:
			unpack_implicit_int(p, e - 4, &m->origid);
			m->parms |= TCAP_PTF_ORIGID;
			unpack_implicit_int(p, e, &m->destid);
			m->parms |= TCAP_PTF_DESTID;
			return (0);
		}
		return (-EPROTO);
	}
	return (-EMSGSIZE);
}

/* ANSI dialog portion */
static int
unpack_priv_dialog_portion(uchar **p, uchar *e, struct tr_msg *m)
{
	t_uscalar_t err;
	t_uscalar_t tag;

	if (*p >= e)
		return (0);
	if (**p != (0xe0 | TCAP_TAG_PRIV_DLGP))
		return (0);
	if ((err = unpack_priv_cons(p, &e, &tag)))
		return (err);
	if (tag != TCAP_TAG_PRIV_DLGP)
		return (-EINVAL);
	m->dlg_beg = *p;
	m->dlg_end = e;
	*p = e;
	m->parms |= TCAP_PTF_DLGP;
	return (0);
}

/* ANSI component portion */
static int
unpack_priv_component_portion(uchar **p, uchar *e, struct tr_msg *m, t_uscalar_t opt)
{
	t_uscalar_t err;
	t_uscalar_t tag = 0; /* XXX */

	if (opt) {
		if (*p >= e)
			return (0);
		if (**p != (0xe0 | TCAP_TAG_PRIV_CSEQ))
			return (0);
	}
	if ((err = unpack_priv_cons(p, &e, &tag)))
		return (err);
	if (tag != TCAP_TAG_PRIV_CSEQ)
		return (-EINVAL);
	m->cmp_beg = *p;
	m->cmp_end = e;
	*p = e;
	m->parms |= TCAP_PTF_CSEQ;
	return (0);
}

/* ANSI abort cause information */
static int
unpack_priv_cause_info(uchar **p, uchar *e, struct tr_msg *m)
{
	(void) p;
	(void) e;
	(void) m;
	/* FIXME */
	return (-EFAULT);
}
static int
tcap_priv_dec_uni(uchar *p, uchar *e, struct tr_msg *m)
{
	int err;

	m->type = TCAP_MT_UNI;
	if ((err = unpack_priv_trsid(&p, e, m, TCAP_MT_UNI)))
		return (err);
	if ((err = unpack_priv_dialog_portion(&p, e, m)))
		return (err);
	if ((err = unpack_priv_component_portion(&p, e, m, 0)))
		return (err);
	return (0);
}
static int
tcap_priv_dec_qwp(uchar *p, uchar *e, struct tr_msg *m)
{
	int err;

	m->type = TCAP_MT_QWP;
	if ((err = unpack_priv_trsid(&p, e, m, TCAP_MT_QWP)))
		return (err);
	if ((err = unpack_priv_dialog_portion(&p, e, m)))
		return (err);
	if ((err = unpack_priv_component_portion(&p, e, m, 1)))
		return (err);
	return (0);
}
static int
tcap_priv_dec_qwop(uchar *p, uchar *e, struct tr_msg *m)
{
	int err;

	m->type = TCAP_MT_QWOP;
	if ((err = unpack_priv_trsid(&p, e, m, TCAP_MT_QWOP)))
		return (err);
	if ((err = unpack_priv_dialog_portion(&p, e, m)))
		return (err);
	if ((err = unpack_priv_component_portion(&p, e, m, 1)))
		return (err);
	return (0);
}
static int
tcap_priv_dec_cwp(uchar *p, uchar *e, struct tr_msg *m)
{
	int err;

	m->type = TCAP_MT_CWP;
	if ((err = unpack_priv_trsid(&p, e, m, TCAP_MT_CWP)))
		return (err);
	if ((err = unpack_priv_dialog_portion(&p, e, m)))
		return (err);
	if ((err = unpack_priv_component_portion(&p, e, m, 1)))
		return (err);
	return (0);
}
static int
tcap_priv_dec_cwop(uchar *p, uchar *e, struct tr_msg *m)
{
	int err;

	m->type = TCAP_MT_CWOP;
	if ((err = unpack_priv_trsid(&p, e, m, TCAP_MT_CWOP)))
		return (err);
	if ((err = unpack_priv_dialog_portion(&p, e, m)))
		return (err);
	if ((err = unpack_priv_component_portion(&p, e, m, 1)))
		return (err);
	return (0);
}
static int
tcap_priv_dec_resp(uchar *p, uchar *e, struct tr_msg *m)
{
	int err;

	m->type = TCAP_MT_RESP;
	if ((err = unpack_priv_trsid(&p, e, m, TCAP_MT_RESP)))
		return (err);
	if ((err = unpack_priv_dialog_portion(&p, e, m)))
		return (err);
	if ((err = unpack_priv_component_portion(&p, e, m, 1)))
		return (err);
	return (0);
}
static int
tcap_priv_dec_abt(uchar *p, uchar *e, struct tr_msg *m)
{
	int err;

	m->type = TCAP_MT_ABORT;
	if ((err = unpack_priv_trsid(&p, e, m, TCAP_MT_ABORT)))
		return (err);
	if ((err = unpack_priv_dialog_portion(&p, e, m)))
		return (err);
	if ((err = unpack_priv_cause_info(&p, e, m)))
		return (err);
	return (0);
}

/*
 *  APPLICATION-TCAP version of Packages.  (Used by the TR sub-layer.)
 *  -------------------------------------------------------------------------
 */
static inline int
unpack_appl_origid(uchar **p, uchar *e, struct tr_msg *m)
{
	t_uscalar_t err;
	t_uscalar_t tag = 0; /* XXX */

	if ((err = unpack_appl_prim(p, &e, &tag)))
		return (err);
	if (tag != TCAP_TAG_APPL_ORIGID)
		return (-EPROTO);
	if (4 < e - *p || e - *p < 1)
		return (-EPROTO);
	unpack_implicit_int(p, e, &m->origid);
	m->parms |= TCAP_PTF_ORIGID;
	return (0);
}
static inline int
unpack_appl_destid(uchar **p, uchar *e, struct tr_msg *m)
{
	t_uscalar_t err;
	t_uscalar_t tag = 0; /* XXX */

	if ((err = unpack_appl_prim(p, &e, &tag)))
		return (err);
	if (tag != TCAP_TAG_APPL_DESTID)
		return (-EPROTO);
	if (4 < e - *p || e - *p < 1)
		return (-EPROTO);
	unpack_implicit_int(p, e, &m->destid);
	m->parms |= TCAP_PTF_DESTID;
	return (0);
}
static inline int
unpack_appl_trsid(uchar **p, uchar *e, struct tr_msg *m, const t_uscalar_t mtype)
{
	t_uscalar_t err;

	switch (mtype) {
	case TCAP_MT_UNI:
		return (0);
	case TCAP_MT_QWP:
		if ((err = unpack_appl_origid(p, e, m)))
			return (err);
		return (0);
	case TCAP_MT_CWP:
		if ((err = unpack_appl_origid(p, e, m)))
			return (err);
		if ((err = unpack_appl_destid(p, e, m)))
			return (err);
		return (0);
	case TCAP_MT_RESP:
	case TCAP_MT_ABORT:
		if ((err = unpack_appl_destid(p, e, m)))
			return (err);
		return (0);
	}
	return (-EFAULT);
}

/* ITU-T dialog portion */
static int
unpack_appl_dialog_portion(uchar **p, uchar *e, struct tr_msg *m)
{
	t_uscalar_t err;
	t_uscalar_t tag;

	if (*p >= e)
		return (0);
	if (**p != (0x60 | TCAP_TAG_APPL_DLGP))
		return (0);
	if ((err = unpack_appl_cons(p, &e, &tag)))
		return (err);
	if (tag != TCAP_TAG_APPL_DLGP)
		return (-EINVAL);
	m->dlg_beg = *p;
	m->dlg_end = e;
	*p = e;
	m->parms |= TCAP_PTF_DLGP;
	return (0);
}

/* ITU-T component portion */
static int
unpack_appl_component_portion(uchar **p, uchar *e, struct tr_msg *m, t_uscalar_t opt)
{
	t_uscalar_t err;
	t_uscalar_t tag = 0; /* XXX */

	if (opt) {
		if (*p >= e)
			return (0);
		if (**p != (0x60 | TCAP_TAG_APPL_CSEQ))
			return (0);
	}
	if ((err = unpack_appl_cons(p, &e, &tag)))
		return (err);
	if (tag != TCAP_TAG_APPL_CSEQ)
		return (-EINVAL);
	m->cmp_beg = *p;
	m->cmp_end = e;
	*p = e;
	m->parms |= TCAP_PTF_CSEQ;
	return (0);
}

/* ITU-T abort cause information */
static int
unpack_appl_cause_info(uchar **p, uchar *e, struct tr_msg *m)
{
	(void) p;
	(void) e;
	(void) m;
	/* FIXME */
	return (-EFAULT);
}
static int
tcap_appl_dec_uni(uchar *p, uchar *e, struct tr_msg *m)
{
	/* Application Context, Component Sequence */
	int err;

	m->type = TCAP_MT_UNI;
	if ((err = unpack_appl_trsid(&p, e, m, TCAP_MT_UNI)))
		return (err);
	if ((err = unpack_appl_dialog_portion(&p, e, m)))
		return (err);
	if ((err = unpack_appl_component_portion(&p, e, m, 0)))
		return (err);
	return (0);
}
static int
tcap_appl_dec_beg(uchar *p, uchar *e, struct tr_msg *m)
{
	/* Originating Transaction Id, Application Context, Component Sequence */
	int err;

	m->type = TCAP_MT_QWP;	/* Query with permission to release */
	if ((err = unpack_appl_trsid(&p, e, m, TCAP_MT_QWP)))
		return (err);
	if ((err = unpack_appl_dialog_portion(&p, e, m)))
		return (err);
	if ((err = unpack_appl_component_portion(&p, e, m, 1)))
		return (err);
	return (0);
}
static int
tcap_appl_dec_cnt(uchar *p, uchar *e, struct tr_msg *m)
{
	int err;

	m->type = TCAP_MT_CWP;	/* Conversation with permission to release */
	if ((err = unpack_appl_trsid(&p, e, m, TCAP_MT_CWP)))
		return (err);
	if ((err = unpack_appl_dialog_portion(&p, e, m)))
		return (err);
	if ((err = unpack_appl_component_portion(&p, e, m, 1)))
		return (err);
	return (0);
}
static int
tcap_appl_dec_end(uchar *p, uchar *e, struct tr_msg *m)
{
	/* Dest Transaction Id, Application Context, Component Sequence */
	int err;

	m->type = TCAP_MT_RESP;	/* Response */
	if ((err = unpack_appl_trsid(&p, e, m, TCAP_MT_RESP)))
		return (err);
	if ((err = unpack_appl_dialog_portion(&p, e, m)))
		return (err);
	if ((err = unpack_appl_component_portion(&p, e, m, 1)))
		return (err);
	return (0);
}
static int
tcap_appl_dec_abt(uchar *p, uchar *e, struct tr_msg *m)
{
	/* Dest Transaction Id, P-Abort-Cause prim or U-Abort-Info const */
	int err;

	if ((err = unpack_appl_trsid(&p, e, m, TCAP_MT_ABORT)))
		return (err);
	if ((err = unpack_appl_dialog_portion(&p, e, m)))
		return (err);
	if ((err = unpack_appl_cause_info(&p, e, m)))
		return (err);
	return (0);
}

/*
 *  TCAP Package decoder.  (Only decodes TR Sub-Layer.)
 *  -------------------------------------------------------------------------
 */
static int
tcap_dec_msg(uchar *p, uchar *e, struct tr_msg *m)
{
	int err;
	t_uscalar_t tag;
	t_uscalar_t cls;

	if ((err = unpack_taglen(&p, &e, &tag, &cls)))
		return (err);
	if ((cls & 0xe0) == 0xe0) {	/* private constructor */
		switch (tag) {
		case TCAP_TAG_PRIV_UNI:	/* Unidirectional */
			return tcap_priv_dec_uni(p, e, m);
		case TCAP_TAG_PRIV_QWP:	/* Begin or Query w/ permission */
			return tcap_priv_dec_qwp(p, e, m);
		case TCAP_TAG_PRIV_QWOP:	/* Query w/o permission */
			return tcap_priv_dec_qwop(p, e, m);
		case TCAP_TAG_PRIV_RESP:	/* End or Response */
			return tcap_priv_dec_resp(p, e, m);
		case TCAP_TAG_PRIV_CWP:	/* Continue or Conversation w/ permission */
			return tcap_priv_dec_cwp(p, e, m);
		case TCAP_TAG_PRIV_CWOP:	/* Conversation w/o permission */
			return tcap_priv_dec_cwop(p, e, m);
		case TCAP_TAG_PRIV_ABORT:	/* Abort */
			return tcap_priv_dec_abt(p, e, m);
		}
	}
	if ((cls & 0xe0) == 0xa0) {	/* application-wide constructor */
		switch (tag) {
		case TCAP_TAG_APPL_UNI:	/* Unidirectional */
			return tcap_appl_dec_uni(p, e, m);
		case TCAP_TAG_APPL_BEGIN:	/* Begin */
			return tcap_appl_dec_beg(p, e, m);
		case TCAP_TAG_APPL_END:	/* End */
			return tcap_appl_dec_end(p, e, m);
		case TCAP_TAG_APPL_CONT:	/* Continue */
			return tcap_appl_dec_cnt(p, e, m);
		case TCAP_TAG_APPL_ABORT:	/* Abort */
			return tcap_appl_dec_abt(p, e, m);
		}
	}
	return (-EINVAL);
}

/*
 *  =========================================================================
 *
 *  TCAP ENCODE Message Functions
 *
 *  =========================================================================
 */
/*
 *  General purpose encoding functions.
 *  -------------------------------------------------------------------------
 */
static inline int
len_size(t_uscalar_t len)
{
	if (!(len & 0xffffff80))
		return (1);
	if (len & 0xff000000)
		return (5);
	if (len & 0xffff0000)
		return (4);
	if (len & 0xffffff00)
		return (3);
	if (len & 0xffffff80)
		return (2);
	return (1);
}
static inline int
tag_size(t_uscalar_t tag)
{
	if (!(tag & 0xffffffc0))
		return (1);
	if (tag & 0xf0000000)
		return (6);
	if (tag & 0xffe00000)
		return (5);
	if (tag & 0xffffc000)
		return (4);
	if (tag & 0xffffff80)
		return (3);
	if (tag & 0xffffffc0)
		return (2);
	return (1);
}
static inline void
pack_tag_class(uchar **p, t_uscalar_t tag, t_uscalar_t fcls)
{
	int n;

	if (tag < 32) {
		*(*p)++ = tag | fcls;
		return;
	}
	n = tag_size(tag) - 1;
	*(*p)++ = 0x1f | fcls;
	switch (n) {
	case 5:
		*(*p)++ = (tag >> 28) | 0x80;
	case 4:
		*(*p)++ = (tag >> 21) | 0x80;
	case 3:
		*(*p)++ = (tag >> 14) | 0x80;
	case 2:
		*(*p)++ = (tag >> 7) | 0x80;
	case 1:
		*(*p)++ = (tag >> 0) | 0x00;
	}
	return;
}
static inline void
pack_tag_univ_prim(uchar **p, t_uscalar_t tag)
{
	return pack_tag_class(p, tag, 0x00);
}
static inline void
pack_tag_univ_cons(uchar **p, t_uscalar_t tag)
{
	return pack_tag_class(p, tag, 0x20);
}
static inline void
pack_tag_appl_prim(uchar **p, t_uscalar_t tag)
{
	return pack_tag_class(p, tag, 0x40);
}
static inline void
pack_tag_appl_cons(uchar **p, t_uscalar_t tag)
{
	return pack_tag_class(p, tag, 0x60);
}
static inline void
pack_tag_cntx_prim(uchar **p, t_uscalar_t tag)
{
	return pack_tag_class(p, tag, 0x80);
}
static inline void
pack_tag_cntx_cons(uchar **p, t_uscalar_t tag)
{
	return pack_tag_class(p, tag, 0xa0);
}
static inline void
pack_tag_priv_prim(uchar **p, t_uscalar_t tag)
{
	return pack_tag_class(p, tag, 0xc0);
}
static inline void
pack_tag_priv_cons(uchar **p, t_uscalar_t tag)
{
	return pack_tag_class(p, tag, 0xe0);
}
static inline void
pack_len(uchar **p, t_uscalar_t len)
{
	t_uscalar_t n;

	if (len < 128) {
		*(*p)++ = len;
		return;
	}
	n = len_size(len) - 1;
	*(*p)++ = 0x80 | n;
	switch (n) {
	case 4:
		*(*p)++ = len >> 24;
	case 3:
		*(*p)++ = len >> 16;
	case 2:
		*(*p)++ = len >> 8;
	case 1:
		*(*p)++ = len >> 0;
	}
	return;
}
static inline void
pack_int(uchar **p, t_uscalar_t val, size_t len)
{
	fixme(("Write this function.\n"));
	return;
}

/*
 *  =========================================================================
 *
 *  TCAP ENCODE Messages (Transaction (TR) Sub-Layer).
 *
 *  =========================================================================
 *
 *  ANSI TCAP Message (Transaction (TR) Sub-Layer) encoding.
 *
 *  -------------------------------------------------------------------------
 *
 *  ANSI UNI (Unidirectional)
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *
tcap_priv_enc_uni(mblk_t *dp, mblk_t *cp)
{
	mblk_t *mp, *mc;
	size_t pkg_len, dlg_len, cmp_len, mlen, hlen, ilen, dlen, clen;

	/* 
	 *  TODO: reduce these to constants where possible
	 */
	dlg_len = dp ? msgdsize(dp) : 0;
	cmp_len = cp ? msgdsize(cp) : 0;
	ilen = tag_size(TCAP_TAG_PRIV_TRSID) + len_size(0);
	dlen = dlg_len ? (tag_size(TCAP_TAG_PRIV_DLGP) + len_size(dlg_len)) : 0;
	clen = cmp_len ? (tag_size(TCAP_TAG_PRIV_CSEQ) + len_size(cmp_len)) : 0;
	pkg_len = ilen + dlen + dlg_len + clen + cmp_len;
	hlen = tag_size(TCAP_TAG_PRIV_UNI) + len_size(pkg_len);
	mlen = hlen + ilen + dlen ? dlen : clen;
	if ((mp = allocb(mlen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		pack_tag_priv_cons(&mp->b_wptr, TCAP_TAG_PRIV_UNI);
		pack_len(&mp->b_wptr, pkg_len);
		pack_tag_priv_prim(&mp->b_wptr, TCAP_TAG_PRIV_TRSID);
		pack_len(&mp->b_wptr, 0);
		if (clen && !dlen) {	/* common case */
			pack_tag_priv_cons(&mp->b_wptr, TCAP_TAG_PRIV_CSEQ);
			pack_len(&mp->b_wptr, cmp_len);
			linkb(mp, cp);
			return (mp);
		}
		if (dlen) {	/* oddball case */
			pack_tag_priv_cons(&mp->b_wptr, TCAP_TAG_PRIV_DLGP);
			pack_len(&mp->b_wptr, dlg_len);
			linkb(mp, dp);
		}
		if (clen) {	/* even odder case */
			if ((mc = allocb(clen, BPRI_MED))) {
				mc->b_datap->db_type = M_DATA;
				pack_tag_priv_cons(&mc->b_wptr, TCAP_TAG_PRIV_CSEQ);
				pack_len(&mc->b_wptr, cmp_len);
				linkb(mp, mc);
				linkb(mp, cp);
				return (mp);
			}
			freeb(mp);
			return (NULL);
		}
	}
	return (mp);
}

/*
 *  ANSI QWP (Query with permission)
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *
tcap_priv_enc_qwp(t_uscalar_t origid, mblk_t *dp, mblk_t *cp)
{
	mblk_t *mp, *mc;
	size_t pkg_len, dlg_len, cmp_len, mlen, hlen, ilen, dlen, clen;

	/* 
	 *  TODO: reduce these to constants where possible
	 */
	dlg_len = dp ? msgdsize(dp) : 0;
	cmp_len = cp ? msgdsize(cp) : 0;
	ilen = tag_size(TCAP_TAG_PRIV_TRSID) + len_size(sizeof(origid));
	dlen = dlg_len ? (tag_size(TCAP_TAG_PRIV_DLGP) + len_size(dlg_len)) : 0;
	clen = cmp_len ? (tag_size(TCAP_TAG_PRIV_CSEQ) + len_size(cmp_len)) : 0;
	pkg_len = ilen + sizeof(origid) + dlen + dlg_len + clen + cmp_len;
	hlen = tag_size(TCAP_TAG_PRIV_QWP) + len_size(pkg_len);
	mlen = hlen + ilen + dlen ? dlen : clen;
	if ((mp = allocb(mlen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		pack_tag_priv_cons(&mp->b_wptr, TCAP_TAG_PRIV_QWP);
		pack_len(&mp->b_wptr, pkg_len);
		pack_tag_priv_prim(&mp->b_wptr, TCAP_TAG_PRIV_TRSID);
		pack_len(&mp->b_wptr, sizeof(origid));
		pack_int(&mp->b_wptr, origid, sizeof(origid));
		if (clen && !dlen) {	/* common case */
			pack_tag_priv_cons(&mp->b_wptr, TCAP_TAG_PRIV_CSEQ);
			pack_len(&mp->b_wptr, cmp_len);
			linkb(mp, cp);
			return (mp);
		}
		if (dlen) {	/* oddball case */
			pack_tag_priv_cons(&mp->b_wptr, TCAP_TAG_PRIV_DLGP);
			pack_len(&mp->b_wptr, dlg_len);
			linkb(mp, dp);
		}
		if (clen) {	/* even odder case */
			if ((mc = allocb(clen, BPRI_MED))) {
				mc->b_datap->db_type = M_DATA;
				pack_tag_priv_cons(&mc->b_wptr, TCAP_TAG_PRIV_CSEQ);
				pack_len(&mc->b_wptr, cmp_len);
				linkb(mp, mc);
				linkb(mp, cp);
				return (mp);
			}
			freeb(mp);
			return (NULL);
		}
	}
	return (mp);
}

/*
 *  ANSI QWOP (Query without permission)
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *
tcap_priv_enc_qwop(t_uscalar_t origid, mblk_t *dp, mblk_t *cp)
{
	mblk_t *mp, *mc;
	size_t pkg_len, dlg_len, cmp_len, mlen, hlen, ilen, dlen, clen;

	/* 
	 *  TODO: reduce these to constants where possible
	 */
	dlg_len = dp ? msgdsize(dp) : 0;
	cmp_len = cp ? msgdsize(cp) : 0;
	ilen = tag_size(TCAP_TAG_PRIV_TRSID) + len_size(sizeof(origid));
	dlen = dlg_len ? (tag_size(TCAP_TAG_PRIV_DLGP) + len_size(dlg_len)) : 0;
	clen = cmp_len ? (tag_size(TCAP_TAG_PRIV_CSEQ) + len_size(cmp_len)) : 0;
	pkg_len = ilen + sizeof(origid) + dlen + dlg_len + clen + cmp_len;
	hlen = tag_size(TCAP_TAG_PRIV_QWOP) + len_size(pkg_len);
	mlen = hlen + ilen + dlen ? dlen : clen;
	if ((mp = allocb(mlen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		pack_tag_priv_cons(&mp->b_wptr, TCAP_TAG_PRIV_QWOP);
		pack_len(&mp->b_wptr, pkg_len);
		pack_tag_priv_prim(&mp->b_wptr, TCAP_TAG_PRIV_TRSID);
		pack_len(&mp->b_wptr, sizeof(origid));
		pack_int(&mp->b_wptr, origid, sizeof(origid));
		if (clen && !dlen) {	/* common case */
			pack_tag_priv_cons(&mp->b_wptr, TCAP_TAG_PRIV_CSEQ);
			pack_len(&mp->b_wptr, cmp_len);
			linkb(mp, cp);
			return (mp);
		}
		if (dlen) {	/* oddball case */
			pack_tag_priv_cons(&mp->b_wptr, TCAP_TAG_PRIV_DLGP);
			pack_len(&mp->b_wptr, dlg_len);
			linkb(mp, dp);
		}
		if (clen) {	/* even odder case */
			if ((mc = allocb(clen, BPRI_MED))) {
				mc->b_datap->db_type = M_DATA;
				pack_tag_priv_cons(&mc->b_wptr, TCAP_TAG_PRIV_CSEQ);
				pack_len(&mc->b_wptr, cmp_len);
				linkb(mp, mc);
				linkb(mp, cp);
				return (mp);
			}
			freeb(mp);
			return (NULL);
		}
	}
	return (mp);
}

/*
 *  ANSI RESP (Response)
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *
tcap_priv_enc_resp(t_uscalar_t destid, mblk_t *dp, mblk_t *cp)
{
	mblk_t *mp, *mc;
	size_t pkg_len, dlg_len, cmp_len, mlen, hlen, ilen, dlen, clen;

	/* 
	 *  TODO: reduce these to constants where possible
	 */
	dlg_len = dp ? msgdsize(dp) : 0;
	cmp_len = cp ? msgdsize(cp) : 0;
	ilen = tag_size(TCAP_TAG_PRIV_TRSID) + len_size(sizeof(destid));
	dlen = dlg_len ? (tag_size(TCAP_TAG_PRIV_DLGP) + len_size(dlg_len)) : 0;
	clen = cmp_len ? (tag_size(TCAP_TAG_PRIV_CSEQ) + len_size(cmp_len)) : 0;
	pkg_len = ilen + sizeof(destid) + dlen + dlg_len + clen + cmp_len;
	hlen = tag_size(TCAP_TAG_PRIV_RESP) + len_size(pkg_len);
	mlen = hlen + ilen + dlen ? dlen : clen;
	if ((mp = allocb(mlen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		pack_tag_priv_cons(&mp->b_wptr, TCAP_TAG_PRIV_RESP);
		pack_len(&mp->b_wptr, pkg_len);
		pack_tag_priv_prim(&mp->b_wptr, TCAP_TAG_PRIV_TRSID);
		pack_len(&mp->b_wptr, sizeof(destid));
		pack_int(&mp->b_wptr, destid, sizeof(destid));
		if (clen && !dlen) {	/* common case */
			pack_tag_priv_cons(&mp->b_wptr, TCAP_TAG_PRIV_CSEQ);
			pack_len(&mp->b_wptr, cmp_len);
			linkb(mp, cp);
			return (mp);
		}
		if (dlen) {	/* oddball case */
			pack_tag_priv_cons(&mp->b_wptr, TCAP_TAG_PRIV_DLGP);
			pack_len(&mp->b_wptr, dlg_len);
			linkb(mp, dp);
		}
		if (clen) {	/* even odder case */
			if ((mc = allocb(clen, BPRI_MED))) {
				mc->b_datap->db_type = M_DATA;
				pack_tag_priv_cons(&mc->b_wptr, TCAP_TAG_PRIV_CSEQ);
				pack_len(&mc->b_wptr, cmp_len);
				linkb(mp, mc);
				linkb(mp, cp);
				return (mp);
			}
			freeb(mp);
			return (NULL);
		}
	}
	return (mp);
}

/*
 *  ANSI CWP (Conversation with permission)
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *
tcap_priv_enc_cwp(t_uscalar_t origid, t_uscalar_t destid, mblk_t *dp, mblk_t *cp)
{
	mblk_t *mp, *mc;
	size_t pkg_len, dlg_len, cmp_len, mlen, hlen, ilen, dlen, clen;

	/* 
	 *  TODO: reduce these to constants where possible
	 */
	dlg_len = dp ? msgdsize(dp) : 0;
	cmp_len = cp ? msgdsize(cp) : 0;
	ilen = tag_size(TCAP_TAG_PRIV_TRSID) + len_size(sizeof(origid) + sizeof(destid));
	dlen = dlg_len ? (tag_size(TCAP_TAG_PRIV_DLGP) + len_size(dlg_len)) : 0;
	clen = cmp_len ? (tag_size(TCAP_TAG_PRIV_CSEQ) + len_size(cmp_len)) : 0;
	pkg_len = ilen + sizeof(origid) + sizeof(destid) + dlen + dlg_len + clen + cmp_len;
	hlen = tag_size(TCAP_TAG_PRIV_CWP) + len_size(pkg_len);
	mlen = hlen + ilen + dlen ? dlen : clen;
	if ((mp = allocb(mlen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		pack_tag_priv_cons(&mp->b_wptr, TCAP_TAG_PRIV_CWP);
		pack_len(&mp->b_wptr, pkg_len);
		pack_tag_priv_prim(&mp->b_wptr, TCAP_TAG_PRIV_TRSID);
		pack_len(&mp->b_wptr, sizeof(origid) + sizeof(destid));
		pack_int(&mp->b_wptr, origid, sizeof(origid));
		pack_int(&mp->b_wptr, destid, sizeof(destid));
		if (clen && !dlen) {	/* common case */
			pack_tag_priv_cons(&mp->b_wptr, TCAP_TAG_PRIV_CSEQ);
			pack_len(&mp->b_wptr, cmp_len);
			linkb(mp, cp);
			return (mp);
		}
		if (dlen) {	/* oddball case */
			pack_tag_priv_cons(&mp->b_wptr, TCAP_TAG_PRIV_DLGP);
			pack_len(&mp->b_wptr, dlg_len);
			linkb(mp, dp);
		}
		if (clen) {	/* even odder case */
			if ((mc = allocb(clen, BPRI_MED))) {
				mc->b_datap->db_type = M_DATA;
				pack_tag_priv_cons(&mc->b_wptr, TCAP_TAG_PRIV_CSEQ);
				pack_len(&mc->b_wptr, cmp_len);
				linkb(mp, mc);
				linkb(mp, cp);
				return (mp);
			}
			freeb(mp);
			return (NULL);
		}
	}
	return (mp);
}

/*
 *  ANSI CWOP (Conversation without permission)
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *
tcap_priv_enc_cwop(t_uscalar_t origid, t_uscalar_t destid, mblk_t *dp, mblk_t *cp)
{
	mblk_t *mp, *mc;
	size_t pkg_len, dlg_len, cmp_len, mlen, hlen, ilen, dlen, clen;

	/* 
	 *  TODO: reduce these to constants where possible
	 */
	dlg_len = dp ? msgdsize(dp) : 0;
	cmp_len = cp ? msgdsize(cp) : 0;
	ilen = tag_size(TCAP_TAG_PRIV_TRSID) + len_size(sizeof(origid) + sizeof(destid));
	dlen = dlg_len ? (tag_size(TCAP_TAG_PRIV_DLGP) + len_size(dlg_len)) : 0;
	clen = cmp_len ? (tag_size(TCAP_TAG_PRIV_CSEQ) + len_size(cmp_len)) : 0;
	pkg_len = ilen + sizeof(origid) + sizeof(destid) + dlen + dlg_len + clen + cmp_len;
	hlen = tag_size(TCAP_TAG_PRIV_CWOP) + len_size(pkg_len);
	mlen = hlen + ilen + dlen ? dlen : clen;
	if ((mp = allocb(mlen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		pack_tag_priv_cons(&mp->b_wptr, TCAP_TAG_PRIV_CWOP);
		pack_len(&mp->b_wptr, pkg_len);
		pack_tag_priv_prim(&mp->b_wptr, TCAP_TAG_PRIV_TRSID);
		pack_len(&mp->b_wptr, sizeof(origid) + sizeof(destid));
		pack_int(&mp->b_wptr, origid, sizeof(origid));
		pack_int(&mp->b_wptr, destid, sizeof(destid));
		if (clen && !dlen) {	/* common case */
			pack_tag_priv_cons(&mp->b_wptr, TCAP_TAG_PRIV_CSEQ);
			pack_len(&mp->b_wptr, cmp_len);
			linkb(mp, cp);
			return (mp);
		}
		if (dlen) {	/* oddball case */
			pack_tag_priv_cons(&mp->b_wptr, TCAP_TAG_PRIV_DLGP);
			pack_len(&mp->b_wptr, dlg_len);
			linkb(mp, dp);
		}
		if (clen) {	/* even odder case */
			if ((mc = allocb(clen, BPRI_MED))) {
				mc->b_datap->db_type = M_DATA;
				pack_tag_priv_cons(&mc->b_wptr, TCAP_TAG_PRIV_CSEQ);
				pack_len(&mc->b_wptr, cmp_len);
				linkb(mp, mc);
				linkb(mp, cp);
				return (mp);
			}
			freeb(mp);
			return (NULL);
		}
	}
	return (mp);
}

/*
 *  ANSI ABORT (Abort)
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *
tcap_priv_enc_abt(t_uscalar_t destid, mblk_t *dp, mblk_t *cp)
{
	mblk_t *mp, *mc;
	size_t pkg_len, dlg_len, cmp_len, mlen, hlen, ilen, dlen, clen;

	/* 
	 *  TODO: reduce these to constants where possible
	 */
	dlg_len = dp ? msgdsize(dp) : 0;
	cmp_len = cp ? msgdsize(cp) : 0;
	ilen = tag_size(TCAP_TAG_PRIV_TRSID) + len_size(sizeof(destid));
	dlen = dlg_len ? (tag_size(TCAP_TAG_PRIV_DLGP) + len_size(dlg_len)) : 0;
	clen = cmp_len ? (tag_size(TCAP_TAG_PRIV_CSEQ) + len_size(cmp_len)) : 0;
	pkg_len = ilen + sizeof(destid) + dlen + dlg_len + clen + cmp_len;
	hlen = tag_size(TCAP_TAG_PRIV_ABORT) + len_size(pkg_len);
	mlen = hlen + ilen + dlen ? dlen : clen;
	if ((mp = allocb(mlen, BPRI_MED))) {
		/* 
		 *  FIXME for cause information rather than components
		 */
		mp->b_datap->db_type = M_DATA;
		pack_tag_priv_cons(&mp->b_wptr, TCAP_TAG_PRIV_ABORT);
		pack_len(&mp->b_wptr, pkg_len);
		pack_tag_priv_prim(&mp->b_wptr, TCAP_TAG_PRIV_TRSID);
		pack_len(&mp->b_wptr, sizeof(destid));
		pack_int(&mp->b_wptr, destid, sizeof(destid));
		if (clen && !dlen) {	/* common case */
			pack_tag_priv_cons(&mp->b_wptr, TCAP_TAG_PRIV_CSEQ);
			pack_len(&mp->b_wptr, cmp_len);
			linkb(mp, cp);
			return (mp);
		}
		if (dlen) {	/* oddball case */
			pack_tag_priv_cons(&mp->b_wptr, TCAP_TAG_PRIV_DLGP);
			pack_len(&mp->b_wptr, dlg_len);
			linkb(mp, dp);
		}
		if (clen) {	/* even odder case */
			if ((mc = allocb(clen, BPRI_MED))) {
				mc->b_datap->db_type = M_DATA;
				pack_tag_priv_cons(&mc->b_wptr, TCAP_TAG_PRIV_CSEQ);
				pack_len(&mc->b_wptr, cmp_len);
				linkb(mp, mc);
				linkb(mp, cp);
				return (mp);
			}
			freeb(mp);
			return (NULL);
		}
	}
	return (mp);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  ITUT TCAP Message (Transaction (TR) Sub-Layer) encoding.
 *
 *  -------------------------------------------------------------------------
 *
 *  ITUT UNI (Unidirectional)
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *
tcap_appl_enc_uni(mblk_t *dp, mblk_t *cp)
{
	mblk_t *mp, *mc;
	size_t pkg_len, dlg_len, cmp_len, mlen, hlen, ilen, dlen, clen;

	/* 
	 *  TODO: reduce these to constants where possible
	 */
	dlg_len = dp ? msgdsize(dp) : 0;
	cmp_len = cp ? msgdsize(cp) : 0;
	ilen = 0;
	dlen = dlg_len ? (tag_size(TCAP_TAG_APPL_DLGP) + len_size(dlg_len)) : 0;
	clen = cmp_len ? (tag_size(TCAP_TAG_APPL_CSEQ) + len_size(cmp_len)) : 0;
	pkg_len = ilen + dlen + dlg_len + clen + cmp_len;
	hlen = tag_size(TCAP_TAG_APPL_UNI) + len_size(pkg_len);
	mlen = hlen + ilen + dlen ? dlen : clen;
	if ((mp = allocb(mlen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		pack_tag_appl_cons(&mp->b_wptr, TCAP_TAG_APPL_UNI);
		pack_len(&mp->b_wptr, pkg_len);
		if (clen && !dlen) {	/* common case, components no dialog */
			pack_tag_appl_cons(&mp->b_wptr, TCAP_TAG_APPL_CSEQ);
			pack_len(&mp->b_wptr, cmp_len);
			linkb(mp, cp);
			return (mp);
		}
		if (dlen) {	/* oddball case, dialog */
			pack_tag_appl_cons(&mp->b_wptr, TCAP_TAG_APPL_DLGP);
			pack_len(&mp->b_wptr, dlg_len);
			linkb(mp, dp);
		}
		if (clen) {	/* oddbal case, dialog and components */
			if ((mc = allocb(clen, BPRI_MED))) {
				mc->b_datap->db_type = M_DATA;
				pack_tag_appl_cons(&mc->b_wptr, TCAP_TAG_APPL_CSEQ);
				pack_len(&mc->b_wptr, cmp_len);
				linkb(mp, mc);
				linkb(mp, cp);
				return (mp);
			}
			freeb(mp);
			return (NULL);
		}
	}
	return (mp);
}

/*
 *  ITUT BEG (Begin)
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *
tcap_appl_enc_beg(t_uscalar_t origid, mblk_t *dp, mblk_t *cp)
{
	mblk_t *mp, *mc;
	size_t pkg_len, dlg_len, cmp_len, mlen, hlen, ilen, dlen, clen;

	/* 
	 *  TODO: reduce these to constants where possible
	 */
	dlg_len = dp ? msgdsize(dp) : 0;
	cmp_len = cp ? msgdsize(cp) : 0;
	ilen = tag_size(TCAP_TAG_APPL_ORIGID) + len_size(sizeof(origid));
	dlen = dlg_len ? (tag_size(TCAP_TAG_APPL_DLGP) + len_size(dlg_len)) : 0;
	clen = cmp_len ? (tag_size(TCAP_TAG_APPL_CSEQ) + len_size(cmp_len)) : 0;
	pkg_len = ilen + sizeof(origid) + dlen + dlg_len + clen + cmp_len;
	hlen = tag_size(TCAP_TAG_APPL_BEGIN) + len_size(pkg_len);
	mlen = hlen + ilen + dlen ? dlen : clen;
	if ((mp = allocb(mlen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		pack_tag_appl_cons(&mp->b_wptr, TCAP_TAG_APPL_BEGIN);
		pack_len(&mp->b_wptr, pkg_len);
		pack_tag_appl_prim(&mp->b_wptr, TCAP_TAG_APPL_ORIGID);
		pack_len(&mp->b_wptr, sizeof(origid));
		pack_int(&mp->b_wptr, origid, sizeof(origid));
		if (clen && !dlen) {	/* common case, components no dialog */
			pack_tag_appl_cons(&mp->b_wptr, TCAP_TAG_APPL_CSEQ);
			pack_len(&mp->b_wptr, cmp_len);
			linkb(mp, cp);
			return (mp);
		}
		if (dlen) {	/* oddball case, dialog */
			pack_tag_appl_cons(&mp->b_wptr, TCAP_TAG_APPL_DLGP);
			pack_len(&mp->b_wptr, dlg_len);
			linkb(mp, dp);
		}
		if (clen) {	/* oddbal case, dialog and components */
			if ((mc = allocb(clen, BPRI_MED))) {
				mc->b_datap->db_type = M_DATA;
				pack_tag_appl_cons(&mc->b_wptr, TCAP_TAG_APPL_CSEQ);
				pack_len(&mc->b_wptr, cmp_len);
				linkb(mp, mc);
				linkb(mp, cp);
				return (mp);
			}
			freeb(mp);
			return (NULL);
		}
	}
	return (mp);
}

/*
 *  ITUT END (End)
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *
tcap_appl_enc_end(t_uscalar_t destid, mblk_t *dp, mblk_t *cp)
{
	mblk_t *mp, *mc;
	size_t pkg_len, dlg_len, cmp_len, mlen, hlen, ilen, dlen, clen;

	/* 
	 *  TODO: reduce these to constants where possible
	 */
	dlg_len = dp ? msgdsize(dp) : 0;
	cmp_len = cp ? msgdsize(cp) : 0;
	ilen = tag_size(TCAP_TAG_APPL_DESTID) + len_size(sizeof(destid));
	dlen = dlg_len ? (tag_size(TCAP_TAG_APPL_DLGP) + len_size(dlg_len)) : 0;
	clen = cmp_len ? (tag_size(TCAP_TAG_APPL_CSEQ) + len_size(cmp_len)) : 0;
	pkg_len = ilen + sizeof(destid) + dlen + dlg_len + clen + cmp_len;
	hlen = tag_size(TCAP_TAG_APPL_END) + len_size(pkg_len);
	mlen = hlen + ilen + dlen ? dlen : clen;
	if ((mp = allocb(mlen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		pack_tag_appl_cons(&mp->b_wptr, TCAP_TAG_APPL_END);
		pack_len(&mp->b_wptr, pkg_len);
		pack_tag_appl_prim(&mp->b_wptr, TCAP_TAG_APPL_DESTID);
		pack_len(&mp->b_wptr, sizeof(destid));
		pack_int(&mp->b_wptr, destid, sizeof(destid));
		if (clen && !dlen) {	/* common case, components no dialog */
			pack_tag_appl_cons(&mp->b_wptr, TCAP_TAG_APPL_CSEQ);
			pack_len(&mp->b_wptr, cmp_len);
			linkb(mp, cp);
			return (mp);
		}
		if (dlen) {	/* oddball case, dialog */
			pack_tag_appl_cons(&mp->b_wptr, TCAP_TAG_APPL_DLGP);
			pack_len(&mp->b_wptr, dlg_len);
			linkb(mp, dp);
		}
		if (clen) {	/* oddbal case, dialog and components */
			if ((mc = allocb(clen, BPRI_MED))) {
				mc->b_datap->db_type = M_DATA;
				pack_tag_appl_cons(&mc->b_wptr, TCAP_TAG_APPL_CSEQ);
				pack_len(&mc->b_wptr, cmp_len);
				linkb(mp, mc);
				linkb(mp, cp);
				return (mp);
			}
			freeb(mp);
			return (NULL);
		}
	}
	return (mp);
}

/*
 *  ITUT CONT (Continue)
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *
tcap_appl_enc_cont(t_uscalar_t origid, t_uscalar_t destid, mblk_t *dp, mblk_t *cp)
{
	mblk_t *mp, *mc;
	size_t pkg_len, dlg_len, cmp_len, mlen, hlen, ilen, dlen, clen;

	/* 
	 *  TODO: reduce these to constants where possible
	 */
	dlg_len = dp ? msgdsize(dp) : 0;
	cmp_len = cp ? msgdsize(cp) : 0;
	ilen =
	    tag_size(TCAP_TAG_APPL_ORIGID) + len_size(sizeof(origid)) +
	    tag_size(TCAP_TAG_APPL_DESTID) + len_size(sizeof(destid));
	dlen = dlg_len ? (tag_size(TCAP_TAG_APPL_DLGP) + len_size(dlg_len)) : 0;
	clen = cmp_len ? (tag_size(TCAP_TAG_APPL_CSEQ) + len_size(cmp_len)) : 0;
	pkg_len = ilen + sizeof(destid) + dlen + dlg_len + clen + cmp_len;
	hlen = tag_size(TCAP_TAG_APPL_CONT) + len_size(pkg_len);
	mlen = hlen + ilen + dlen ? dlen : clen;
	if ((mp = allocb(mlen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		pack_tag_appl_cons(&mp->b_wptr, TCAP_TAG_APPL_CONT);
		pack_len(&mp->b_wptr, pkg_len);
		pack_tag_appl_prim(&mp->b_wptr, TCAP_TAG_APPL_ORIGID);
		pack_len(&mp->b_wptr, sizeof(origid));
		pack_int(&mp->b_wptr, origid, sizeof(origid));
		pack_tag_appl_prim(&mp->b_wptr, TCAP_TAG_APPL_DESTID);
		pack_len(&mp->b_wptr, sizeof(destid));
		pack_int(&mp->b_wptr, destid, sizeof(destid));
		if (clen && !dlen) {	/* common case, components no dialog */
			pack_tag_appl_cons(&mp->b_wptr, TCAP_TAG_APPL_CSEQ);
			pack_len(&mp->b_wptr, cmp_len);
			linkb(mp, cp);
			return (mp);
		}
		if (dlen) {	/* oddball case, dialog */
			pack_tag_appl_cons(&mp->b_wptr, TCAP_TAG_APPL_DLGP);
			pack_len(&mp->b_wptr, dlg_len);
			linkb(mp, dp);
		}
		if (clen) {	/* oddbal case, dialog and components */
			if ((mc = allocb(clen, BPRI_MED))) {
				mc->b_datap->db_type = M_DATA;
				pack_tag_appl_cons(&mc->b_wptr, TCAP_TAG_APPL_CSEQ);
				pack_len(&mc->b_wptr, cmp_len);
				linkb(mp, mc);
				linkb(mp, cp);
				return (mp);
			}
			freeb(mp);
			return (NULL);
		}
	}
	return (mp);
}

/*
 *  ITUT ABORT (Abort)
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *
tcap_appl_enc_abt(t_uscalar_t destid, mblk_t *dp, mblk_t *cp)
{
	mblk_t *mp, *mc;
	size_t pkg_len, dlg_len, cmp_len, mlen, hlen, ilen, dlen, clen;

	/* 
	 *  TODO: reduce these to constants where possible
	 */
	dlg_len = dp ? msgdsize(dp) : 0;
	cmp_len = cp ? msgdsize(cp) : 0;
	ilen = tag_size(TCAP_TAG_APPL_DESTID) + len_size(sizeof(destid));
	dlen = dlg_len ? (tag_size(TCAP_TAG_APPL_DLGP) + len_size(dlg_len)) : 0;
	clen = cmp_len ? (tag_size(TCAP_TAG_APPL_CSEQ) + len_size(cmp_len)) : 0;
	pkg_len = ilen + sizeof(destid) + dlen + dlg_len + clen + cmp_len;
	hlen = tag_size(TCAP_TAG_APPL_ABORT) + len_size(pkg_len);
	mlen = hlen + ilen + dlen ? dlen : clen;
	if ((mp = allocb(mlen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		pack_tag_appl_cons(&mp->b_wptr, TCAP_TAG_APPL_ABORT);
		pack_len(&mp->b_wptr, pkg_len);
		pack_tag_appl_prim(&mp->b_wptr, TCAP_TAG_APPL_DESTID);
		pack_len(&mp->b_wptr, sizeof(destid));
		pack_int(&mp->b_wptr, destid, sizeof(destid));
		/* 
		 *  FIXME for cause information rather than components
		 */
		if (clen && !dlen) {	/* common case, components no dialog */
			pack_tag_appl_cons(&mp->b_wptr, TCAP_TAG_APPL_CSEQ);
			pack_len(&mp->b_wptr, cmp_len);
			linkb(mp, cp);
			return (mp);
		}
		if (dlen) {	/* oddball case, dialog */
			pack_tag_appl_cons(&mp->b_wptr, TCAP_TAG_APPL_DLGP);
			pack_len(&mp->b_wptr, dlg_len);
			linkb(mp, dp);
		}
		if (clen) {	/* oddbal case, dialog and components */
			if ((mc = allocb(clen, BPRI_MED))) {
				mc->b_datap->db_type = M_DATA;
				pack_tag_appl_cons(&mc->b_wptr, TCAP_TAG_APPL_CSEQ);
				pack_len(&mc->b_wptr, cmp_len);
				linkb(mp, mc);
				linkb(mp, cp);
				return (mp);
			}
			freeb(mp);
			return (NULL);
		}
	}
	return (mp);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  ANSI TCAP Component (Component (TC) Sub-Layer) encoding.
 *
 *  -------------------------------------------------------------------------
 *
 *  ANSI INKL (Invoke Last)
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *
tcap_priv_enc_inkl(t_uscalar_t *iid, t_uscalar_t *lid, t_uscalar_t opclass, t_uscalar_t opcode,
		   mblk_t *pseq)
{
	mblk_t *mp = NULL;

	return (mp);
}

/*
 *  ANSI INK  (Invoke Not Last)
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *
tcap_priv_enc_ink(t_uscalar_t *iid, t_uscalar_t *lid, t_uscalar_t opclass, t_uscalar_t opcode,
		  mblk_t *pseq)
{
	mblk_t *mp = NULL;

	return (mp);
}

/*
 *  ANSI RRL  (Return Result Last)
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *
tcap_priv_enc_rrl(t_uscalar_t *iid, mblk_t *pseq)
{
	mblk_t *mp = NULL;

	return (mp);
}

/*
 *  ANSI RR   (Return Result Not Last)
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *
tcap_priv_enc_rr(t_uscalar_t *iid, mblk_t *pseq)
{
	mblk_t *mp = NULL;

	return (mp);
}

/*
 *  ANSI RER  (Return Error)
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *
tcap_priv_enc_rer(t_uscalar_t *iid, t_uscalar_t ecode, mblk_t *pseq)
{
	mblk_t *mp = NULL;

	return (mp);
}

/*
 *  ANSI REJ  (Reject)
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *
tcap_priv_enc_rej(t_uscalar_t *iid, t_uscalar_t pcode, mblk_t *pseq)
{
	mblk_t *mp = NULL;

	return (mp);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  ITUT TCAP Component (Component (TC) Sub-Layer) encoding.
 *
 *  -------------------------------------------------------------------------
 *
 *  ITUT INKL (Invoke Last)
 *  -------------------------------------------------------------------------
 */
/*
 *  ITUT RRL  (Return Result Last)
 *  -------------------------------------------------------------------------
 */
/*
 *  ITUT RR   (Return Result Not Last)
 *  -------------------------------------------------------------------------
 */
/*
 *  ITUT RER  (Return Error)
 *  -------------------------------------------------------------------------
 */
/*
 *  ITUT REJ  (Reject)
 *  -------------------------------------------------------------------------
 */

/*
 *  =========================================================================
 *
 *  OPTION Handling
 *
 *  =========================================================================
 */

typedef struct tcap_var {
	uchar *ptr;
	size_t len;
} tcap_var_t;

typedef struct tcap_opts {
	ulong flags;			/* success flags */
	tcap_var_t ctx;			/* application context */
	tcap_var_t inf;			/* user information */
	t_uscalar_t *pcl;		/* protocol class */
	t_uscalar_t *ret;		/* return option */
	t_uscalar_t *imp;		/* importance */
	t_uscalar_t *seq;		/* sequence control */
	t_uscalar_t *pri;		/* priority */
	t_uscalar_t *sls;		/* signalling link selection */
	t_uscalar_t *mp;		/* message priority */
} tcap_opts_t;

#define TF_OPT_APP	(1<<0)	/* application context */
#define TF_OPT_INF	(1<<1)	/* user information */
#define TF_OPT_PCL	(1<<2)	/* protocol class */
#define TF_OPT_RET	(1<<3)	/* return option */
#define TF_OPT_IMP	(1<<4)	/* importance */
#define TF_OPT_SEQ	(1<<5)	/* sequence control */
#define TF_OPT_PRI	(1<<6)	/* priority */
#define TF_OPT_SLS	(1<<7)	/* signalling link selection */
#define TF_OPT_MP	(1<<8)	/* message priority */

#define _T_ALIGN_SIZEOF(s) \
	((sizeof((s)) + _T_ALIGN_SIZE - 1) & ~(_T_ALIGN_SIZE - 1))
static size_t
tcap_opts_size(struct tc *tc, struct tcap_opts *ops)
{
	size_t len = 0;

	if (ops) {
		const size_t hlen = sizeof(struct t_opthdr);	/* 32 bytes */

		if (ops->ctx.ptr)
			len += hlen + ops->ctx.len;
		if (ops->inf.ptr)
			len += hlen + ops->inf.len;
		if (ops->pcl)
			len += hlen + _T_ALIGN_SIZEOF(*(ops->pcl));
		if (ops->ret)
			len += hlen + _T_ALIGN_SIZEOF(*(ops->ret));
		if (ops->imp)
			len += hlen + _T_ALIGN_SIZEOF(*(ops->imp));
		if (ops->seq)
			len += hlen + _T_ALIGN_SIZEOF(*(ops->seq));
		if (ops->pri)
			len += hlen + _T_ALIGN_SIZEOF(*(ops->pri));
		if (ops->sls)
			len += hlen + _T_ALIGN_SIZEOF(*(ops->sls));
		if (ops->mp)
			len += hlen + _T_ALIGN_SIZEOF(*(ops->mp));
	}
	return (len);
}
static void
tcap_build_opts(struct tc *tc, struct tcap_opts *ops, uchar *p)
{
	if (ops) {
		struct t_opthdr *oh;
		const size_t hlen = sizeof(struct t_opthdr);

		if (ops->ctx.ptr) {
			oh = (typeof(oh)) p;
			oh->len = hlen + ops->ctx.len;
			oh->level = T_SS7_TCAP;
			oh->name = T_TCAP_APP_CTX;
			oh->status = (ops->flags & TF_OPT_APP) ? T_SUCCESS : T_FAILURE;
			p += sizeof(*oh);
			bcopy(ops->ctx.ptr, p, ops->ctx.len);
			p += T_ALIGN(ops->ctx.len);
		}
		if (ops->inf.ptr) {
			oh = (typeof(oh)) p;
			oh->len = hlen + ops->inf.len;
			oh->level = T_SS7_TCAP;
			oh->name = T_TCAP_USER_INFO;
			oh->status = (ops->flags & TF_OPT_INF) ? T_SUCCESS : T_FAILURE;
			p += sizeof(*oh);
			bcopy(ops->inf.ptr, p, ops->inf.len);
			p += T_ALIGN(ops->inf.len);
		}
		if (ops->pcl) {
			oh = (typeof(oh)) p;
			oh->len = hlen + sizeof(*(ops->pcl));
			oh->level = T_SS7_SCCP;
			oh->name = T_SCCP_PCLASS;
			oh->status = (ops->flags & TF_OPT_PCL) ? T_SUCCESS : T_FAILURE;
			p += sizeof(*oh);
			*((typeof(ops->pcl)) p) = *(ops->pcl);
			p += _T_ALIGN_SIZEOF(*ops->pcl);
		}
		if (ops->ret) {
			oh = (typeof(oh)) p;
			oh->len = hlen + sizeof(*(ops->ret));
			oh->level = T_SS7_SCCP;
			oh->name = T_SCCP_RET_ERROR;
			oh->status = (ops->flags & TF_OPT_RET) ? T_SUCCESS : T_FAILURE;
			p += sizeof(*oh);
			*((typeof(ops->ret)) p) = *(ops->ret);
			p += _T_ALIGN_SIZEOF(*ops->ret);
		}
		if (ops->imp) {
			oh = (typeof(oh)) p;
			oh->len = hlen + sizeof(*(ops->imp));
			oh->level = T_SS7_SCCP;
			oh->name = T_SCCP_IMPORTANCE;
			oh->status = (ops->flags & TF_OPT_IMP) ? T_SUCCESS : T_FAILURE;
			p += sizeof(*oh);
			*((typeof(ops->imp)) p) = *(ops->imp);
			p += _T_ALIGN_SIZEOF(*ops->imp);
		}
		if (ops->seq) {
			oh = (typeof(oh)) p;
			oh->len = hlen + sizeof(*(ops->seq));
			oh->level = T_SS7_SCCP;
			oh->name = T_SCCP_SEQ_CTRL;
			oh->status = (ops->flags & TF_OPT_SEQ) ? T_SUCCESS : T_FAILURE;
			p += sizeof(*oh);
			*((typeof(ops->seq)) p) = *(ops->seq);
			p += _T_ALIGN_SIZEOF(*ops->seq);
		}
		if (ops->pri) {
			oh = (typeof(oh)) p;
			oh->len = hlen + sizeof(*(ops->pri));
			oh->level = T_SS7_SCCP;
			oh->name = T_SCCP_PRIORITY;
			oh->status = (ops->flags & TF_OPT_PRI) ? T_SUCCESS : T_FAILURE;
			p += sizeof(*oh);
			*((typeof(ops->pri)) p) = *(ops->pri);
			p += _T_ALIGN_SIZEOF(*ops->pri);
		}
		if (ops->sls) {
			oh = (typeof(oh)) p;
			oh->len = hlen + sizeof(*(ops->sls));
			oh->level = T_SS7_MTP;
			oh->name = T_MTP_SLS;
			oh->status = (ops->flags & TF_OPT_SLS) ? T_SUCCESS : T_FAILURE;
			p += sizeof(*oh);
			*((typeof(ops->sls)) p) = *(ops->sls);
			p += _T_ALIGN_SIZEOF(*ops->sls);
		}
		if (ops->mp) {
			oh = (typeof(oh)) p;
			oh->len = hlen + sizeof(*(ops->mp));
			oh->level = T_SS7_MTP;
			oh->name = T_MTP_MP;
			oh->status = (ops->flags & TF_OPT_MP) ? T_SUCCESS : T_FAILURE;
			p += sizeof(*oh);
			*((typeof(ops->mp)) p) = *(ops->mp);
			p += _T_ALIGN_SIZEOF(*ops->mp);
		}
	}
}
static int
tcap_parse_opts(struct tc *tc, struct tcap_opts *ops, uchar *op, size_t len)
{
	struct t_opthdr *oh;

	for (oh = _T_OPT_FIRSTHDR_OFS(op, len, 0); oh; oh = _T_OPT_NEXTHDR_OFS(op, len, oh, 0)) {
		switch (oh->level) {
		case T_SS7_TCAP:
			switch (oh->name) {
			case T_TCAP_APP_CTX:
				ops->ctx.ptr = (void *) _T_OPT_DATA_OFS(oh, 0);
				ops->ctx.len = oh->len - sizeof(*oh);
				ops->flags |= TF_OPT_APP;
				continue;
			case T_TCAP_USER_INFO:
				ops->inf.ptr = (void *) _T_OPT_DATA_OFS(oh, 0);
				ops->inf.len = oh->len - sizeof(*oh);
				ops->flags |= TF_OPT_INF;
				continue;
			}
			break;
		case T_SS7_SCCP:
			switch (oh->name) {
			case T_SCCP_PCLASS:
				ops->pcl = (void *) _T_OPT_DATA_OFS(oh, 0);
				ops->flags |= TF_OPT_PCL;
				continue;
			case T_SCCP_RET_ERROR:
				ops->ret = (void *) _T_OPT_DATA_OFS(oh, 0);
				ops->flags |= TF_OPT_RET;
				continue;
			case T_SCCP_IMPORTANCE:
				ops->imp = (void *) _T_OPT_DATA_OFS(oh, 0);
				ops->flags |= TF_OPT_IMP;
				continue;
			case T_SCCP_SEQ_CTRL:
				ops->seq = (void *) _T_OPT_DATA_OFS(oh, 0);
				ops->flags |= TF_OPT_SEQ;
				continue;
			case T_SCCP_PRIORITY:
				ops->pri = (void *) _T_OPT_DATA_OFS(oh, 0);
				ops->flags |= TF_OPT_PRI;
				continue;
			}
			break;
		case T_SS7_MTP:
			switch (oh->name) {
			case T_MTP_SLS:
				ops->sls = (void *) _T_OPT_DATA_OFS(oh, 0);
				ops->flags |= TF_OPT_SLS;
				continue;
			case T_MTP_MP:
				ops->mp = (void *) _T_OPT_DATA_OFS(oh, 0);
				ops->flags |= TF_OPT_MP;
				continue;
			}
			break;
			break;
		}
	}
	if (oh)
		return (TBADOPT);
	return (0);
}

/*
 *  =========================================================================
 *
 *  OPTIONS handling
 *
 *  =========================================================================
 */
static inline int
tcap_opt_check(struct tc *tc, struct tcap_opts *ops)
{
	if (ops && ops->flags) {
		ops->flags = 0;
		if (ops->ctx.ptr && ops->ctx.len <= TCAP_BUF_MAXLEN)
			ops->flags |= TF_OPT_APP;
		if (ops->inf.ptr && ops->inf.len <= TCAP_BUF_MAXLEN)
			ops->flags |= TF_OPT_INF;
		if (ops->pcl)
			ops->flags |= TF_OPT_PCL;
		if (ops->ret)
			ops->flags |= TF_OPT_RET;
		if (ops->imp)
			ops->flags |= TF_OPT_IMP;
		if (ops->seq)
			ops->flags |= TF_OPT_SEQ;
		if (ops->pri)
			ops->flags |= TF_OPT_PRI;
		if (ops->sls)
			ops->flags |= TF_OPT_SLS;
		if (ops->mp)
			ops->flags |= TF_OPT_MP;
	}
	return (0);
}
static inline int
tcap_opt_default(struct tc *tc, struct tcap_opts *ops)
{
	if (ops) {
		int flags = ops->flags;

		ops->flags = 0;
		if (!flags || ops->ctx.ptr) {
			ops->ctx.ptr = opt_defaults.ctx.buf;
			ops->ctx.len = opt_defaults.ctx.len;
			ops->flags |= TF_OPT_APP;
		}
		if (!flags || ops->inf.ptr) {
			ops->inf.ptr = opt_defaults.inf.buf;
			ops->inf.len = opt_defaults.inf.len;
			ops->flags |= TF_OPT_INF;
		}
		if (!flags || ops->pcl) {
			ops->pcl = &opt_defaults.pcl;
			ops->flags |= TF_OPT_PCL;
		}
		if (!flags || ops->ret) {
			ops->ret = &opt_defaults.ret;
			ops->flags |= TF_OPT_RET;
		}
		if (!flags || ops->imp) {
			ops->imp = &opt_defaults.imp;
			ops->flags |= TF_OPT_IMP;
		}
		if (!flags || ops->seq) {
			ops->seq = &opt_defaults.seq;
			ops->flags |= TF_OPT_SEQ;
		}
		if (!flags || ops->pri) {
			ops->pri = &opt_defaults.pri;
			ops->flags |= TF_OPT_PRI;
		}
		if (!flags || ops->sls) {
			ops->sls = &opt_defaults.sls;
			ops->flags |= TF_OPT_SLS;
		}
		if (!flags || ops->mp) {
			ops->mp = &opt_defaults.mp;
			ops->flags |= TF_OPT_MP;
		}
		return (0);
	}
	swerr();
	return (-EFAULT);
}
static inline int
tcap_opt_current(struct tc *tc, struct tcap_opts *ops)
{
	int flags = ops->flags;

	ops->flags = 0;
	if (!flags || ops->ctx.ptr) {
		ops->ctx.ptr = tc->options.ctx.buf;
		ops->ctx.len = tc->options.ctx.len;
		ops->flags |= TF_OPT_APP;
	}
	if (!flags || ops->inf.ptr) {
		ops->inf.ptr = tc->options.inf.buf;
		ops->inf.len = tc->options.inf.len;
		ops->flags |= TF_OPT_INF;
	}
	if (!flags || ops->pcl) {
		ops->pcl = &tc->options.pcl;
		ops->flags |= TF_OPT_PCL;
	}
	if (!flags || ops->ret) {
		ops->ret = &tc->options.ret;
		ops->flags |= TF_OPT_RET;
	}
	if (!flags || ops->imp) {
		ops->imp = &tc->options.imp;
		ops->flags |= TF_OPT_IMP;
	}
	if (!flags || ops->seq) {
		ops->seq = &tc->options.seq;
		ops->flags |= TF_OPT_SEQ;
	}
	if (!flags || ops->pri) {
		ops->pri = &tc->options.pri;
		ops->flags |= TF_OPT_PRI;
	}
	if (!flags || ops->sls) {
		ops->sls = &tc->options.sls;
		ops->flags |= TF_OPT_SLS;
	}
	if (!flags || ops->mp) {
		ops->mp = &tc->options.mp;
		ops->flags |= TF_OPT_MP;
	}
	return (0);
}
static inline int
tcap_opt_negotiate(struct tc *tc, struct tcap_opts *ops)
{
	if (ops->flags) {
		ops->flags = 0;
		if (ops->ctx.ptr && ops->ctx.len <= TCAP_BUF_MAXLEN) {
			bcopy(ops->ctx.ptr, tc->options.ctx.buf, ops->ctx.len);
			tc->options.ctx.len = ops->ctx.len;
			ops->ctx.ptr = tc->options.ctx.buf;
			ops->flags |= TF_OPT_APP;
		}
		if (ops->inf.ptr && ops->inf.len <= TCAP_BUF_MAXLEN) {
			bcopy(ops->inf.ptr, tc->options.inf.buf, ops->inf.len);
			tc->options.inf.len = ops->inf.len;
			ops->inf.ptr = tc->options.inf.buf;
			ops->flags |= TF_OPT_INF;
		}
		if (ops->pcl) {
			tc->options.pcl = *ops->pcl;
			ops->pcl = &tc->options.pcl;
			ops->flags |= TF_OPT_PCL;
		}
		if (ops->ret) {
			tc->options.ret = *ops->ret;
			ops->ret = &tc->options.ret;
			ops->flags |= TF_OPT_RET;
		}
		if (ops->imp) {
			tc->options.imp = *ops->imp;
			ops->imp = &tc->options.imp;
			ops->flags |= TF_OPT_IMP;
		}
		if (ops->seq) {
			tc->options.seq = *ops->seq;
			ops->seq = &tc->options.seq;
			ops->flags |= TF_OPT_SEQ;
		}
		if (ops->pri) {
			tc->options.pri = *ops->pri;
			ops->pri = &tc->options.pri;
			ops->flags |= TF_OPT_PRI;
		}
		if (ops->sls) {
			tc->options.sls = *ops->sls;
			ops->sls = &tc->options.sls;
			ops->flags |= TF_OPT_SLS;
		}
		if (ops->mp) {
			tc->options.mp = *ops->mp;
			ops->mp = &tc->options.mp;
			ops->flags |= TF_OPT_MP;
		}
	}
	return (0);
}

/*
 *  =========================================================================
 *
 *  STATE Changes
 *
 *  =========================================================================
 */

static inline const char *
tcap_r_state(uint state)
{
	switch (state) {
	case TRS_UNBND:
		return ("TRS_UNBND");
	case TRS_WACK_BREQ:
		return ("TRS_WACK_BREQ");
	case TRS_WACK_UREQ:
		return ("TRS_WACK_UREQ");
	case TRS_IDLE:
		return ("TRS_IDLE");
	case TRS_WACK_OPTREQ:
		return ("TRS_WACK_OPTREQ");
	case TRS_WACK_RRES:
		return ("TRS_WACK_RRES");
	case TRS_WCON_CREQ:
		return ("TRS_WCON_CREQ");
	case TRS_WRES_CIND:
		return ("TRS_WRES_CIND");
	case TRS_WACK_CRES:
		return ("TRS_WACK_CRES");
	case TRS_DATA_XFER:
		return ("TRS_DATA_XFER");
	case TRS_WCON_RREQ:
		return ("TRS_WCON_RREQ");
	case TRS_WRES_RIND:
		return ("TRS_WRES_RIND");
	case TRS_WACK_DREQ6:
		return ("TRS_WACK_DREQ6");
	case TRS_WACK_DREQ7:
		return ("TRS_WACK_DREQ7");
	case TRS_WACK_DREQ9:
		return ("TRS_WACK_DREQ9");
	case TRS_WACK_DREQ10:
		return ("TRS_WACK_DREQ10");
	case TRS_WACK_DREQ11:
		return ("TRS_WACK_DREQ11");
	default:
		return ("(unknown)");
	}
}
static inline const char *
tcap_c_state(uint state)
{
	switch (state) {
	case TCS_UNBND:
		return ("TCS_UNBND");
	case TCS_WACK_BREQ:
		return ("TCS_WACK_BREQ");
	case TCS_WACK_UREQ:
		return ("TCS_WACK_UREQ");
	case TCS_IDLE:
		return ("TCS_IDLE");
	case TCS_WACK_OPTREQ:
		return ("TCS_WACK_OPTREQ");
	case TCS_WACK_RRES:
		return ("TCS_WACK_RRES");
	case TCS_WCON_CREQ:
		return ("TCS_WCON_CREQ");
	case TCS_WRES_CIND:
		return ("TCS_WRES_CIND");
	case TCS_WACK_CRES:
		return ("TCS_WACK_CRES");
	case TCS_DATA_XFER:
		return ("TCS_DATA_XFER");
	case TCS_WCON_RREQ:
		return ("TCS_WCON_RREQ");
	case TCS_WRES_RIND:
		return ("TCS_WRES_RIND");
	case TCS_WACK_DREQ6:
		return ("TCS_WACK_DREQ6");
	case TCS_WACK_DREQ7:
		return ("TCS_WACK_DREQ7");
	case TCS_WACK_DREQ9:
		return ("TCS_WACK_DREQ9");
	case TCS_WACK_DREQ10:
		return ("TCS_WACK_DREQ10");
	case TCS_WACK_DREQ11:
		return ("TCS_WACK_DREQ11");
	default:
		return ("(unknown)");
	}
}
static inline const char *
tcap_t_state(uint state)
{
	switch (state) {
	case TS_UNBND:
		return ("TS_UNBND");
	case TS_WACK_BREQ:
		return ("TS_WACK_BREQ");
	case TS_WACK_UREQ:
		return ("TS_WACK_UREQ");
	case TS_IDLE:
		return ("TS_IDLE");
	case TS_WACK_OPTREQ:
		return ("TS_WACK_OPTREQ");
	case TS_WACK_CREQ:
		return ("TS_WACK_CREQ");
	case TS_WCON_CREQ:
		return ("TS_WCON_CREQ");
	case TS_WRES_CIND:
		return ("TS_WRES_CIND");
	case TS_WACK_CRES:
		return ("TS_WACK_CRES");
	case TS_DATA_XFER:
		return ("TS_DATA_XFER");
	case TS_WIND_ORDREL:
		return ("TS_WIND_ORDREL");
	case TS_WREQ_ORDREL:
		return ("TS_WREQ_ORDREL");
	case TS_WACK_DREQ6:
		return ("TS_WACK_DREQ6");
	case TS_WACK_DREQ7:
		return ("TS_WACK_DREQ7");
	case TS_WACK_DREQ9:
		return ("TS_WACK_DREQ9");
	case TS_WACK_DREQ10:
		return ("TS_WACK_DREQ10");
	case TS_WACK_DREQ11:
		return ("TS_WACK_DREQ11");
	default:
		return ("(unknown)");
	}
}
static inline const char *
tcap_m_state(uint state)
{
	switch (state) {
	default:
		return ("(unknown)");
	}
}
static inline const char *
sccp_i_state(uint state)
{
	switch (state) {
	case NS_UNBND:
		return ("NS_UNBND");
	case NS_WACK_BREQ:
		return ("NS_WACK_BREQ");
	case NS_WACK_UREQ:
		return ("NS_WACK_UREQ");
	case NS_IDLE:
		return ("NS_IDLE");
	case NS_WACK_OPTREQ:
		return ("NS_WACK_OPTREQ");
	case NS_WACK_RRES:
		return ("NS_WACK_RRES");
	case NS_WCON_CREQ:
		return ("NS_WCON_CREQ");
	case NS_WRES_CIND:
		return ("NS_WRES_CIND");
	case NS_WACK_CRES:
		return ("NS_WACK_CRES");
	case NS_DATA_XFER:
		return ("NS_DATA_XFER");
	case NS_WCON_RREQ:
		return ("NS_WCON_RREQ");
	case NS_WRES_RIND:
		return ("NS_WRES_RIND");
	case NS_WACK_DREQ6:
		return ("NS_WACK_DREQ6");
	case NS_WACK_DREQ7:
		return ("NS_WACK_DREQ7");
	case NS_WACK_DREQ9:
		return ("NS_WACK_DREQ9");
	case NS_WACK_DREQ10:
		return ("NS_WACK_DREQ10");
	case NS_WACK_DREQ11:
		return ("NS_WACK_DREQ11");
	default:
		return ("(unknown)");
	}
}

static inline void
tcap_set_state(struct tc *tc, uint newstate)
{
	uint oldstate = tc->state;

	(void) oldstate;
	printd(("%s: %p: %s <- %s\n", DRV_NAME, tc, tcap_state(newstate), tcap_state(oldstate)));
	tc->state = newstate;
}
static inline uint
tcap_get_state(struct tc *tc)
{
	return tc->state;
}
static inline void
tcap_set_r_state(struct tc *tc, uint newstate)
{
	uint oldstate = tc->i_state;

	(void) oldstate;
	printd(("%s: %p: %s <- %s\n", DRV_NAME, tc, tcap_r_state(newstate),
		tcap_r_state(oldstate)));
	tc->i_state = newstate;
}
static inline uint
tcap_get_r_state(struct tc *tc)
{
	return tc->i_state;
}
static inline void
tcap_save_r_state(struct tc *tc)
{
	tc->i_oldstate = tcap_get_r_state(tc);
}
static inline void
tcap_restore_r_state(struct tc *tc)
{
	tcap_set_r_state(tc, tc->i_oldstate);
}
static inline void
tcap_set_c_state(struct tc *tc, uint newstate)
{
	uint oldstate = tc->i_state;

	(void) oldstate;
	printd(("%s: %p: %s <- %s\n", DRV_NAME, tc, tcap_c_state(newstate),
		tcap_c_state(oldstate)));
	tc->i_state = newstate;
}
static inline uint
tcap_get_c_state(struct tc *tc)
{
	return tc->i_state;
}
static inline void
tcap_save_c_state(struct tc *tc)
{
	tc->i_oldstate = tcap_get_c_state(tc);
}
static inline void
tcap_restore_c_state(struct tc *tc)
{
	tcap_set_c_state(tc, tc->i_oldstate);
}

static inline void
tcap_set_t_state(struct tc *tc, uint newstate)
{
	uint oldstate = tc->i_state;

	(void) oldstate;
	printd(("%s: %p: %s <- %s\n", DRV_NAME, tc, tcap_t_state(newstate),
		tcap_t_state(oldstate)));
	tc->i_state = newstate;
}
static inline uint
tcap_get_t_state(struct tc *tc)
{
	return tc->i_state;
}
static inline void
tcap_save_t_state(struct tc *tc)
{
	tc->i_oldstate = tcap_get_t_state(tc);
}
static inline void
tcap_restore_t_state(struct tc *tc)
{
	tcap_set_t_state(tc, tc->i_oldstate);
}
static inline void
tcap_set_m_state(struct tc *tc, uint newstate)
{
	uint oldstate = tc->i_state;

	(void) oldstate;
	printd(("%s: %p: %s <- %s\n", DRV_NAME, tc, tcap_m_state(newstate),
		tcap_m_state(oldstate)));
	tc->i_state = newstate;
}
static inline uint
tcap_get_m_state(struct tc *tc)
{
	return tc->i_state;
}
static inline void
tcap_save_m_state(struct tc *tc)
{
	tc->i_oldstate = tcap_get_c_state(tc);
}
static inline void
tcap_restore_m_state(struct tc *tc)
{
	tcap_set_c_state(tc, tc->i_oldstate);
}

static inline void
sccp_set_i_state(struct sc *sc, uint newstate)
{
	uint oldstate = sc->i_state;

	(void) oldstate;
	printd(("%s: %p: %s <- %s\n", DRV_NAME, sc, sccp_i_state(newstate),
		sccp_i_state(oldstate)));
	sc->i_state = newstate;
}
static inline uint
sccp_get_i_state(struct sc *sc)
{
	return sc->i_state;
}

static inline void
sccp_save_i_state(struct sc *sc)
{
	sc->i_oldstate = sccp_get_i_state(sc);
}
static inline void
sccp_restore_i_state(struct sc *sc)
{
	sccp_set_i_state(sc, sc->i_oldstate);
}

/*
 *  TLI interface state flags
 */
#define TSF_UNBND	( 1 << TS_UNBND		)
#define TSF_WACK_BREQ	( 1 << TS_WACK_BREQ	)
#define TSF_WACK_UREQ	( 1 << TS_WACK_UREQ	)
#define TSF_IDLE	( 1 << TS_IDLE		)
#define TSF_WACK_OPTREQ	( 1 << TS_WACK_OPTREQ	)
#define TSF_WACK_CREQ	( 1 << TS_WACK_CREQ	)
#define TSF_WCON_CREQ	( 1 << TS_WCON_CREQ	)
#define TSF_WRES_CIND	( 1 << TS_WRES_CIND	)
#define TSF_WACK_CRES	( 1 << TS_WACK_CRES	)
#define TSF_DATA_XFER	( 1 << TS_DATA_XFER	)
#define TSF_WIND_ORDREL	( 1 << TS_WIND_ORDREL	)
#define TSF_WREQ_ORDREL	( 1 << TS_WREQ_ORDREL	)
#define TSF_WACK_DREQ6	( 1 << TS_WACK_DREQ6	)
#define TSF_WACK_DREQ7	( 1 << TS_WACK_DREQ7	)
#define TSF_WACK_DREQ9	( 1 << TS_WACK_DREQ9	)
#define TSF_WACK_DREQ10	( 1 << TS_WACK_DREQ10	)
#define TSF_WACK_DREQ11	( 1 << TS_WACK_DREQ11	)
#define TSF_NOSTATES	( 1 << TS_NOSTATES	)

#define TSF_WACK_DREQ	(TSF_WACK_DREQ6 \
			|TSF_WACK_DREQ7 \
			|TSF_WACK_DREQ9 \
			|TSF_WACK_DREQ10 \
			|TSF_WACK_DREQ11)

/* 
 *  =========================================================================
 *
 *  PRIMITIVES
 *
 *  =========================================================================
 */
/* 
 *  -------------------------------------------------------------------------
 *
 *  Primitives sent upstream
 *
 *  -------------------------------------------------------------------------
 */

/* 
 *  M_FLUSH
 *  -----------------------------------
 */
/**
 * m_flush: - issue an M_FLUSH message upstream
 * @tc: TCAP private structure
 * @q: active queue
 * @band: band number
 * @flags: read or write flags (FLUSHR | FLUSHW | FLUSHRW)
 */
static noinline __unlikely int
m_flush(queue_t *q, queue_t *pq, int band, int flags)
{
	mblk_t *mp;

	(void) m_flush;
	if (!(mp = mi_allocb(q, 2, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_FLUSH;
	*(mp->b_wptr)++ = flags | (band ? FLUSHBAND : 0);
	*(mp->b_wptr)++ = band;
	printd(("%s: %p: <- M_FLUSH\n", DRV_NAME, pq));
	putq(pq, mp);
	return (QR_DONE);
      enobufs:
	rare();
	return (-ENOBUFS);
}

/* 
 *  M_ERROR
 *  -----------------------------------
 */
static inline int
m_error(struct tc *tc, queue_t *q, int error)
{
	mblk_t *mp;
	int hangup = 0;

	if (error < 0)
		error = -error;
	switch (error) {
	case EBUSY:
	case ENOBUFS:
	case ENOMEM:
	case EAGAIN:
		return (-error);
	case EPIPE:
	case ENETDOWN:
	case EHOSTUNREACH:
		hangup = 1;
	}
	if (!(mp = mi_allocb(q, 2, BPRI_MED)))
		goto enobufs;
	if (hangup) {
		mp->b_datap->db_type = M_HANGUP;
		tcap_set_state(tc, NS_NOSTATES);
		printd(("%s: %p: <- M_HANGUP\n", DRV_NAME, tc));
		put(tc->oq, mp);
		return (-error);
	} else {
		mp->b_datap->db_type = M_ERROR;
		*(mp->b_wptr)++ = error < 0 ? -error : error;
		*(mp->b_rptr)++ = error < 0 ? -error : error;
		tcap_set_state(tc, NS_NOSTATES);
		printd(("%s: %p: <- M_ERROR\n", DRV_NAME, tc));
		put(tc->oq, mp);
		return (QR_DONE);
	}
      enobufs:
	rare();
	return (-ENOBUFS);
}

/* 
 *  TCI Primitives
 *  -------------------------------------------------------------------------
 */
/*
 *  TC_INFO_ACK
 *  -----------------------------------------------------------------
 */
static inline int
tc_info_ack(struct tc *tc, queue_t *q)
{
	int err;
	mblk_t *mp;
	struct TC_info_ack *p;
	size_t msg_len = sizeof(*p);

	if (!(mp = mi_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->PRIM_type = TC_INFO_ACK;
	p->TSDU_size = FIXME;
	p->ETSDU_size = FIXME;
	p->CDATA_size = FIXME;
	p->DDATA_size = FIXME;
	p->ADDR_size = sizeof(struct sccp_addr) + SCCP_MAX_ADDR_LENGTH;
	p->OPT_size = FIXME;
	p->TIDU_size = FIXME;
	p->SERV_type = FIXME;
	p->CURRENT_state = tcap_get_c_state(tc);
	p->PROVIDER_flag = FIXME;
	p->TCI_version = FIXME;
	printd(("%s: %p: <- TC_INFO_ACK\n", DRV_NAME, tc));
	put(tc->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  TC_BIND_ACK
 *  -----------------------------------------------------------------
 */
static inline int
tc_bind_ack(struct tc *tc, queue_t *q, struct sccp_addr *add, t_uscalar_t xact, t_uscalar_t tok)
{
	int err;
	mblk_t *mp;
	struct TC_bind_ack *p;
	size_t add_len = add ? sizeof(*add) + add->alen : 0;
	size_t msg_len = sizeof(*p) + add_len;

	if (!(mp = mi_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->PRIM_type = TC_BIND_ACK;
	p->ADDR_length = add_len;
	p->ADDR_offset = add_len ? sizeof(*p) : 0;
	p->XACT_number = xact;
	p->TOKEN_value = tok;
	if (add_len) {
		bcopy(add, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
	}
	printd(("%s: %p: <- TC_BIND_ACK\n", DRV_NAME, tc));
	put(tc->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  TC_SUBS_BIND_ACK
 *  -----------------------------------------------------------------
 */
static inline int
tc_subs_bind_ack(struct tc *tc, queue_t *q)
{
	int err;
	mblk_t *mp;
	struct TC_subs_bind_ack *p;
	size_t msg_len = sizeof(*p);

	if (!(mp = mi_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->PRIM_type = TC_SUBS_BIND_ACK;
	printd(("%s: %p: <- TC_SUBS_BIND_ACK\n", DRV_NAME, tc));
	put(tc->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  TC_OK_ACK
 *  -----------------------------------------------------------------
 */
static inline int
tc_ok_ack(struct tc *tc, queue_t *q, t_uscalar_t prim)
{
	int err;
	mblk_t *mp;
	struct TC_ok_ack *p;
	size_t msg_len = sizeof(*p);

	if (!(mp = mi_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->PRIM_type = TC_OK_ACK;
	p->CORRECT_prim = prim;
	printd(("%s: %p: <- TC_OK_ACK\n", DRV_NAME, tc));
	put(tc->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  TC_ERROR_ACK
 *  -----------------------------------------------------------------
 */
static inline int
tc_error_ack(struct tc *tc, queue_t *q, t_uscalar_t prim, t_uscalar_t etype, t_uscalar_t did,
	     t_uscalar_t iid)
{
	int err;
	mblk_t *mp;
	struct TC_error_ack *p;
	size_t msg_len = sizeof(*p);

	if (!(mp = mi_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->PRIM_type = TC_ERROR_ACK;
	p->ERROR_prim = prim;
	p->TRPI_error = etype < 0 ? TCSYSERR : etype;
	p->UNIX_error = etype < 0 ? -etype : 0;
	p->DIALOG_id = did;
	p->INVOKE_id = iid;
	printd(("%s: %p: <- TC_ERROR_ACK\n", DRV_NAME, tc));
	put(tc->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  TC_OPTMGMT_ACK
 *  -----------------------------------------------------------------
 */
static inline int
tc_optmgmt_ack(struct tc *tc, queue_t *q, struct tcap_opts *opt, t_uscalar_t flags)
{
	int err;
	mblk_t *mp;
	struct TC_optmgmt_ack *p;
	size_t opt_len = tcap_opts_size(tc, opt);
	size_t msg_len = sizeof(*p) + opt_len;

	if (!(mp = mi_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->PRIM_type = TC_OPTMGMT_ACK;
	p->OPT_length = opt_len;
	p->OPT_offset = opt_len ? sizeof(*p) : 0;
	p->MGMT_flags = flags;
	if (opt_len) {
		tcap_build_opts(tc, opt, mp->b_wptr);
		mp->b_wptr += opt_len;
	}
	printd(("%s: %p: <- TC_OPTMGMT_ACK\n", DRV_NAME, tc));
	put(tc->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  TC_UNI_IND
 *  -----------------------------------------------------------------
 */
static inline int
tc_uni_ind(struct tc *tc, queue_t *q, struct sccp_addr *src, struct sccp_addr *dst,
	   struct tcap_opts *opt, t_uscalar_t did, t_uscalar_t flags, mblk_t *dp)
{
	int err;
	mblk_t *mp;
	struct TC_uni_ind *p;
	size_t src_len = src ? sizeof(*src) + src->alen : 0;
	size_t dst_len = dst ? sizeof(*dst) + dst->alen : 0;
	size_t opt_len = tcap_opts_size(tc, opt);
	size_t msg_len = sizeof(*p) + src_len + dst_len + opt_len;

	if (!canput(tc->oq))
		goto ebusy;
	if (!(mp = mi_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->PRIM_type = TC_UNI_IND;
	p->SRC_length = src_len;
	p->SRC_offset = src_len ? sizeof(*p) : 0;
	p->DEST_length = dst_len;
	p->DEST_offset = dst_len ? sizeof(*p) + src_len : 0;
	p->OPT_length = opt_len;
	p->OPT_offset = opt_len ? sizeof(*p) + src_len + dst_len : 0;
	p->DIALOG_id = did;
	p->COMP_flags = flags;
	if (src_len) {
		bcopy(src, mp->b_wptr, src_len);
		mp->b_wptr += src_len;
	}
	if (dst_len) {
		bcopy(dst, mp->b_wptr, dst_len);
		mp->b_wptr += dst_len;
	}
	if (opt_len) {
		tcap_build_opts(tc, opt, mp->b_wptr);
		mp->b_wptr += opt_len;
	}
	mp->b_cont = dp;
	printd(("%s: %p: <- TC_UNI_IND\n", DRV_NAME, tc));
	put(tc->oq, mp);
	return (QR_ABSORBED);
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      ebusy:
	err = -EBUSY;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  TC_BEGIN_IND
 *  -----------------------------------------------------------------
 */
static inline int
tc_begin_ind(struct tc *tc, queue_t *q, struct sccp_addr *src, struct sccp_addr *dst,
	     struct tcap_opts *opt, t_uscalar_t did, t_uscalar_t flags, mblk_t *dp)
{
	int err;
	mblk_t *mp;
	struct TC_begin_ind *p;
	size_t src_len = src ? sizeof(*src) + src->alen : 0;
	size_t dst_len = dst ? sizeof(*dst) + dst->alen : 0;
	size_t opt_len = tcap_opts_size(tc, opt);
	size_t msg_len = sizeof(*p) + src_len + dst_len + opt_len;

	if (!canput(tc->oq))
		goto ebusy;
	if (!(mp = mi_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->PRIM_type = TC_BEGIN_IND;
	p->SRC_length = src_len;
	p->SRC_offset = src_len ? sizeof(*p) : 0;
	p->DEST_length = dst_len;
	p->DEST_offset = dst_len ? sizeof(*p) + src_len : 0;
	p->OPT_length = opt_len;
	p->OPT_offset = opt_len ? sizeof(*p) + src_len + dst_len : 0;
	p->DIALOG_id = did;
	p->COMP_flags = flags;
	if (src_len) {
		bcopy(src, mp->b_wptr, src_len);
		mp->b_wptr += src_len;
	}
	if (dst_len) {
		bcopy(dst, mp->b_wptr, dst_len);
		mp->b_wptr += dst_len;
	}
	if (opt_len) {
		tcap_build_opts(tc, opt, mp->b_wptr);
		mp->b_wptr += opt_len;
	}
	mp->b_cont = dp;
	printd(("%s: %p: <- TC_BEGIN_IND\n", DRV_NAME, tc));
	put(tc->oq, mp);
	return (QR_ABSORBED);
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      ebusy:
	err = -EBUSY;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  TC_BEGIN_CON
 *  -----------------------------------------------------------------
 */
static inline int
tc_begin_con(struct tc *tc, queue_t *q, struct tcap_opts *opt, t_uscalar_t did,
	     t_uscalar_t flags, mblk_t *dp)
{
	int err;
	mblk_t *mp;
	struct TC_begin_con *p;
	size_t opt_len = tcap_opts_size(tc, opt);
	size_t msg_len = sizeof(*p) + opt_len;

	if (!canput(tc->oq))
		goto ebusy;
	if (!(mp = mi_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->PRIM_type = TC_BEGIN_CON;
	p->OPT_length = opt_len;
	p->OPT_offset = opt_len ? sizeof(*p) : 0;
	p->DIALOG_id = did;
	p->COMP_flags = flags;
	if (opt_len) {
		tcap_build_opts(tc, opt, mp->b_wptr);
		mp->b_wptr += opt_len;
	}
	mp->b_cont = dp;
	printd(("%s: %p: <- TC_BEGIN_CON\n", DRV_NAME, tc));
	put(tc->oq, mp);
	return (QR_ABSORBED);
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      ebusy:
	err = -EBUSY;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  TC_CONT_IND
 *  -----------------------------------------------------------------
 */
static inline int
tc_cont_ind(struct tc *tc, queue_t *q, struct tcap_opts *opt, t_uscalar_t did,
	    t_uscalar_t flags, mblk_t *dp)
{
	int err;
	mblk_t *mp;
	struct TC_cont_ind *p;
	size_t opt_len = tcap_opts_size(tc, opt);
	size_t msg_len = sizeof(*p) + opt_len;

	if (!canput(tc->oq))
		goto ebusy;
	if (!(mp = mi_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->PRIM_type = TC_CONT_IND;
	p->OPT_length = opt_len;
	p->OPT_offset = opt_len ? sizeof(*p) : 0;
	p->DIALOG_id = did;
	p->COMP_flags = flags;
	if (opt_len) {
		tcap_build_opts(tc, opt, mp->b_wptr);
		mp->b_wptr += opt_len;
	}
	mp->b_cont = dp;
	printd(("%s: %p: <- TC_CONT_IND\n", DRV_NAME, tc));
	put(tc->oq, mp);
	return (QR_ABSORBED);
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      ebusy:
	err = -EBUSY;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  TC_END_IND
 *  -----------------------------------------------------------------
 */
static inline int
tc_end_ind(struct tc *tc, queue_t *q, struct tcap_opts *opt, t_uscalar_t did, t_uscalar_t flags,
	   mblk_t *dp)
{
	int err;
	mblk_t *mp;
	struct TC_end_ind *p;
	size_t opt_len = tcap_opts_size(tc, opt);
	size_t msg_len = sizeof(*p) + opt_len;

	if (!canput(tc->oq))
		goto ebusy;
	if (!(mp = mi_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->PRIM_type = TC_END_IND;
	p->OPT_length = opt_len;
	p->OPT_offset = opt_len ? sizeof(*p) : 0;
	p->DIALOG_id = did;
	p->COMP_flags = flags;
	if (opt_len) {
		tcap_build_opts(tc, opt, mp->b_wptr);
		mp->b_wptr += opt_len;
	}
	mp->b_cont = dp;
	printd(("%s: %p: <- TC_END_IND\n", DRV_NAME, tc));
	put(tc->oq, mp);
	return (QR_ABSORBED);
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      ebusy:
	err = -EBUSY;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  TC_ABORT_IND
 *  -----------------------------------------------------------------
 */
static inline int
tc_abort_ind(struct tc *tc, queue_t *q, struct tcap_opts *opt, t_uscalar_t did,
	     t_uscalar_t reason, t_uscalar_t orig)
{
	int err;
	mblk_t *mp;
	struct TC_abort_ind *p;
	size_t opt_len = tcap_opts_size(tc, opt);
	size_t msg_len = sizeof(*p) + opt_len;

	if (!canput(tc->oq))
		goto ebusy;
	if (!(mp = mi_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->PRIM_type = TC_ABORT_IND;
	p->OPT_length = opt_len;
	p->OPT_offset = opt_len ? sizeof(*p) : 0;
	p->DIALOG_id = did;
	p->ABORT_reason = reason;
	p->ORIGINATOR = orig;
	if (opt_len) {
		tcap_build_opts(tc, opt, mp->b_wptr);
		mp->b_wptr += opt_len;
	}
	printd(("%s: %p: <- TC_ABORT_IND\n", DRV_NAME, tc));
	put(tc->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      ebusy:
	err = -EBUSY;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  TC_NOTICE_IND
 *  -----------------------------------------------------------------
 */
static inline int
tc_notice_ind(struct tc *tc, queue_t *q, t_uscalar_t did, t_uscalar_t cause)
{
	int err;
	mblk_t *mp;
	struct TC_notice_ind *p;
	size_t msg_len = sizeof(*p);

	if (!canput(tc->oq))
		goto ebusy;
	if (!(mp = mi_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->PRIM_type = TC_NOTICE_IND;
	p->DIALOG_id = did;
	p->REPORT_cause = cause;
	printd(("%s: %p: <- TC_NOTICE_IND\n", DRV_NAME, tc));
	put(tc->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      ebusy:
	err = -EBUSY;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  TC_INVOKE_IND
 *  -----------------------------------------------------------------
 */
static inline int
tc_invoke_ind(struct tc *tc, queue_t *q, t_uscalar_t did, t_uscalar_t ocls, t_uscalar_t iid,
	      t_uscalar_t lid, t_uscalar_t oper, t_uscalar_t more)
{
	int err;
	mblk_t *mp;
	struct TC_invoke_ind *p;
	size_t msg_len = sizeof(*p);

	if (!canput(tc->oq))
		goto ebusy;
	if (!(mp = mi_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->PRIM_type = TC_INVOKE_IND;
	p->DIALOG_id = did;
	p->OP_class = ocls;
	p->INVOKE_id = iid;
	p->LINKED_id = lid;
	p->OPERATION = oper;
	p->MORE_flag = more;
	printd(("%s: %p: <- TC_INVOKE_IND\n", DRV_NAME, tc));
	put(tc->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      ebusy:
	err = -EBUSY;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  TC_RESULT_IND
 *  -----------------------------------------------------------------
 */
static inline int
tc_result_ind(struct tc *tc, queue_t *q, t_uscalar_t did, t_uscalar_t iid, t_uscalar_t oper,
	      t_uscalar_t more)
{
	int err;
	mblk_t *mp;
	struct TC_result_ind *p;
	size_t msg_len = sizeof(*p);

	if (!canput(tc->oq))
		goto ebusy;
	if (!(mp = mi_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->PRIM_type = TC_RESULT_IND;
	p->DIALOG_id = did;
	p->INVOKE_id = iid;
	p->OPERATION = oper;
	p->MORE_flag = more;
	printd(("%s: %p: <- TC_RESULT_IND\n", DRV_NAME, tc));
	put(tc->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      ebusy:
	err = -EBUSY;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  TC_ERROR_IND
 *  -----------------------------------------------------------------
 */
static inline int
tc_error_ind(struct tc *tc, queue_t *q, t_uscalar_t did, t_uscalar_t iid, t_uscalar_t ecode)
{
	int err;
	mblk_t *mp;
	struct TC_error_ind *p;
	size_t msg_len = sizeof(*p);

	if (!canput(tc->oq))
		goto ebusy;
	if (!(mp = mi_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->PRIM_type = TC_ERROR_IND;
	p->DIALOG_id = did;
	p->INVOKE_id = iid;
	p->ERROR_code = ecode;
	printd(("%s: %p: <- TC_ERROR_IND\n", DRV_NAME, tc));
	put(tc->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      ebusy:
	err = -EBUSY;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  TC_CANCEL_IND
 *  -----------------------------------------------------------------
 */
static inline int
tc_cancel_ind(struct tc *tc, queue_t *q, t_uscalar_t did, t_uscalar_t iid)
{
	int err;
	mblk_t *mp;
	struct TC_cancel_ind *p;
	size_t msg_len = sizeof(*p);

	if (!canput(tc->oq))
		goto ebusy;
	if (!(mp = mi_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->PRIM_type = TC_CANCEL_IND;
	p->DIALOG_id = did;
	p->INVOKE_id = iid;
	printd(("%s: %p: <- TC_CANCEL_IND\n", DRV_NAME, tc));
	put(tc->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      ebusy:
	err = -EBUSY;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  TC_REJECT_IND
 *  -----------------------------------------------------------------
 */
static inline int
tc_reject_ind(struct tc *tc, queue_t *q, t_uscalar_t did, t_uscalar_t iid, t_uscalar_t ecode)
{
	int err;
	mblk_t *mp;
	struct TC_reject_ind *p;
	size_t msg_len = sizeof(*p);

	if (!canput(tc->oq))
		goto ebusy;
	if (!(mp = mi_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->PRIM_type = TC_REJECT_IND;
	p->DIALOG_id = did;
	p->INVOKE_id = iid;
	p->PROBLEM_code = ecode;
	printd(("%s: %p: <- TC_REJECT_IND\n", DRV_NAME, tc));
	put(tc->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      ebusy:
	err = -EBUSY;
	rare();
	goto error;
      error:
	return (err);
}

/* 
 *  TRI Primitives
 *  -------------------------------------------------------------------------
 */
/*
 *  TR_INFO_ACK
 *  -----------------------------------------------------------------
 */
static inline int
tr_info_ack(struct tc *tc, queue_t *q)
{
	int err;
	mblk_t *mp;
	struct TR_info_ack *p;
	size_t msg_len = sizeof(*p);

	if (!(mp = mi_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->PRIM_type = TR_INFO_ACK;
	p->TSDU_size = FIXME;
	p->ETSDU_size = FIXME;
	p->CDATA_size = FIXME;
	p->DDATA_size = FIXME;
	p->ADDR_size = sizeof(struct sccp_addr) + SCCP_MAX_ADDR_LENGTH;
	p->OPT_size = FIXME;
	p->TIDU_size = FIXME;
	p->SERV_type = FIXME;
	p->CURRENT_state = tcap_get_c_state(tc);
	p->PROVIDER_flag = FIXME;
	p->TRPI_version = FIXME;
	printd(("%s: %p: <- TR_INFO_ACK\n", DRV_NAME, tc));
	put(tc->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  TR_BIND_ACK
 *  -----------------------------------------------------------------
 */
static inline int
tr_bind_ack(struct tc *tc, queue_t *q, struct sccp_addr *add, t_uscalar_t xact, t_uscalar_t tok)
{
	int err;
	mblk_t *mp;
	struct TR_bind_ack *p;
	size_t add_len = add ? sizeof(*add) + add->alen : 0;
	size_t msg_len = sizeof(*p) + add_len;

	if (!(mp = mi_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->PRIM_type = TR_BIND_ACK;
	p->ADDR_length = add_len;
	p->ADDR_offset = add_len ? sizeof(*p) : 0;
	p->XACT_number = xact;
	p->TOKEN_value = tok;
//      p->BIND_flags = flags;
	if (add_len) {
		bcopy(add, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
	}
	printd(("%s: %p: <- TR_BIND_ACK\n", DRV_NAME, tc));
	put(tc->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  TR_OK_ACK
 *  -----------------------------------------------------------------
 */
static inline int
tr_ok_ack(struct tc *tc, queue_t *q, t_uscalar_t prim)
{
	int err;
	mblk_t *mp;
	struct TR_ok_ack *p;
	size_t msg_len = sizeof(*p);

	if (!(mp = mi_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->PRIM_type = TR_OK_ACK;
	p->CORRECT_prim = prim;
	printd(("%s: %p: <- TR_OK_ACK\n", DRV_NAME, tc));
	put(tc->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  TR_ERROR_ACK
 *  -----------------------------------------------------------------
 */
static int
tr_error_ack(struct tc *tc, queue_t *q, t_uscalar_t prim, t_uscalar_t etype, t_uscalar_t tid)
{
	int err;
	mblk_t *mp;
	struct TR_error_ack *p;
	size_t msg_len = sizeof(*p);

	if (!(mp = mi_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->PRIM_type = TR_ERROR_ACK;
	p->ERROR_prim = prim;
	p->TRPI_error = etype < 0 ? TRSYSERR : etype;
	p->UNIX_error = etype < 0 ? -etype : 0;
	p->TRANS_id = tid;
	printd(("%s: %p: <- TR_ERROR_ACK\n", DRV_NAME, tc));
	put(tc->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  TR_OPTMGMT_ACK
 *  -----------------------------------------------------------------
 */
static inline int
tr_optmgmt_ack(struct tc *tc, queue_t *q, struct tcap_opts *opt, t_uscalar_t flags)
{
	int err;
	mblk_t *mp;
	struct TR_optmgmt_ack *p;
	size_t opt_len = tcap_opts_size(tc, opt);
	size_t msg_len = sizeof(*p) + opt_len;

	if (!(mp = mi_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->PRIM_type = TR_OPTMGMT_ACK;
	p->OPT_length = opt_len;
	p->OPT_offset = opt_len ? sizeof(*p) : 0;
	p->MGMT_flags = flags;
	if (opt_len) {
		tcap_build_opts(tc, opt, mp->b_wptr);
		mp->b_wptr += opt_len;
	}
	printd(("%s: %p: <- TR_OPTMGMT_ACK\n", DRV_NAME, tc));
	put(tc->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  TR_UNI_IND
 *  -----------------------------------------------------------------
 */
static int
tr_uni_ind(struct tc *tc, queue_t *q, struct sccp_addr *org, struct sccp_addr *dst,
	   struct tcap_opts *opt, mblk_t *dp)
{
	int err;
	mblk_t *mp;
	struct TR_uni_ind *p;
	size_t dst_len = dst ? sizeof(*dst) + dst->alen : 0;
	size_t org_len = org ? sizeof(*org) + org->alen : 0;
	size_t opt_len = tcap_opts_size(tc, opt);
	size_t msg_len = sizeof(*p) + dst_len + org_len + opt_len;

	if (!canput(tc->oq))
		goto ebusy;
	if (!(mp = mi_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->PRIM_type = TR_UNI_IND;
	p->DEST_length = dst_len;
	p->DEST_offset = dst_len ? sizeof(*p) : 0;
	p->ORIG_length = org_len;
	p->ORIG_offset = org_len ? sizeof(*p) + dst_len : 0;
	p->OPT_length = opt_len;
	p->OPT_offset = opt_len ? sizeof(*p) + dst_len + org_len : 0;
	if (dst_len) {
		bcopy(dst, mp->b_wptr, dst_len);
		mp->b_wptr += dst_len;
	}
	if (org_len) {
		bcopy(org, mp->b_wptr, org_len);
		mp->b_wptr += org_len;
	}
	if (opt_len) {
		tcap_build_opts(tc, opt, mp->b_wptr);
		mp->b_wptr += opt_len;
	}
	mp->b_cont = dp;
	printd(("%s: %p: <- TR_UNI_IND\n", DRV_NAME, tc));
	put(tc->oq, mp);
	return (QR_ABSORBED);
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      ebusy:
	err = -EBUSY;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  TR_BEGIN_IND
 *  -----------------------------------------------------------------
 */
static int
tr_begin_ind(struct tc *tc, queue_t *q, struct sccp_addr *org,
	     struct sccp_addr *dst, struct tcap_opts *opt, t_uscalar_t tid, mblk_t *dp)
{
	int err;
	mblk_t *mp;
	struct TR_begin_ind *p;
	size_t dst_len = dst ? sizeof(*dst) + dst->alen : 0;
	size_t org_len = org ? sizeof(*org) + org->alen : 0;
	size_t opt_len = tcap_opts_size(tc, opt);
	size_t msg_len = sizeof(*p) + dst_len + org_len + opt_len;

	if (!canput(tc->oq))
		goto ebusy;
	if (!(mp = mi_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->PRIM_type = TR_BEGIN_IND;
	p->TRANS_id = tid;
	p->DEST_length = dst_len;
	p->DEST_offset = dst_len ? sizeof(*p) : 0;
	p->ORIG_length = org_len;
	p->ORIG_offset = org_len ? sizeof(*p) + dst_len : 0;
	p->OPT_length = opt_len;
	p->OPT_offset = opt_len ? sizeof(*p) + dst_len + org_len : 0;
	if (dst_len) {
		bcopy(dst, mp->b_wptr, dst_len);
		mp->b_wptr += dst_len;
	}
	if (org_len) {
		bcopy(org, mp->b_wptr, org_len);
		mp->b_wptr += org_len;
	}
	if (opt_len) {
		tcap_build_opts(tc, opt, mp->b_wptr);
		mp->b_wptr += opt_len;
	}
	mp->b_cont = dp;
	printd(("%s: %p: <- TR_BEGIN_IND\n", DRV_NAME, tc));
	put(tc->oq, mp);
	return (QR_ABSORBED);
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      ebusy:
	err = -EBUSY;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  TR_BEGIN_CON
 *  -----------------------------------------------------------------
 */
static int
tr_begin_con(struct tc *tc, queue_t *q, struct sccp_addr *org,
	     struct tcap_opts *opt, t_uscalar_t tid, t_uscalar_t cid, mblk_t *dp)
{
	int err;
	mblk_t *mp;
	struct TR_begin_con *p;
	size_t org_len = org ? sizeof(*org) + org->alen : 0;
	size_t opt_len = tcap_opts_size(tc, opt);
	size_t msg_len = sizeof(*p) + org_len + opt_len;

	if (!canput(tc->oq))
		goto ebusy;
	if (!(mp = mi_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->PRIM_type = TR_BEGIN_CON;
	p->CORR_id = cid;
	p->TRANS_id = tid;
	p->ORIG_length = org_len;
	p->ORIG_offset = org_len ? sizeof(*p) : 0;
	p->OPT_length = opt_len;
	p->OPT_offset = opt_len ? sizeof(*p) + org_len : 0;
	if (org_len) {
		bcopy(org, mp->b_wptr, org_len);
		mp->b_wptr += org_len;
	}
	if (opt_len) {
		tcap_build_opts(tc, opt, mp->b_wptr);
		mp->b_wptr += opt_len;
	}
	mp->b_cont = dp;
	printd(("%s: %p: <- TR_BEGIN_CON\n", DRV_NAME, tc));
	put(tc->oq, mp);
	return (QR_ABSORBED);
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      ebusy:
	err = -EBUSY;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  TR_CONT_IND
 *  -----------------------------------------------------------------
 */
static inline int
tr_cont_ind(struct tc *tc, queue_t *q, struct tcap_opts *opt, t_uscalar_t tid, mblk_t *dp)
{
	int err;
	mblk_t *mp;
	struct TR_cont_ind *p;
	size_t opt_len = tcap_opts_size(tc, opt);
	size_t msg_len = sizeof(*p) + opt_len;

	if (!canput(tc->oq))
		goto ebusy;
	if (!(mp = mi_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->PRIM_type = TR_CONT_IND;
	p->TRANS_id = tid;
	p->OPT_length = opt_len;
	p->OPT_offset = opt_len ? sizeof(*p) : 0;
	if (opt_len) {
		tcap_build_opts(tc, opt, mp->b_wptr);
		mp->b_wptr += opt_len;
	}
	mp->b_cont = dp;
	printd(("%s: %p: <- TR_CONT_IND\n", DRV_NAME, tc));
	put(tc->oq, mp);
	return (QR_ABSORBED);
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      ebusy:
	err = -EBUSY;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  TR_END_IND
 *  -----------------------------------------------------------------
 */
static int
tr_end_ind(struct tc *tc, queue_t *q, struct tcap_opts *opt, t_uscalar_t tid, t_uscalar_t cid,
	   mblk_t *dp)
{
	int err;
	mblk_t *mp;
	struct TR_end_ind *p;
	size_t opt_len = tcap_opts_size(tc, opt);
	size_t msg_len = sizeof(*p) + opt_len;

	if (!canput(tc->oq))
		goto ebusy;
	if (!(mp = mi_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->PRIM_type = TR_END_IND;
	p->CORR_id = cid;
	p->TRANS_id = tid;
	p->OPT_length = opt_len;
	p->OPT_offset = opt_len ? sizeof(*p) : 0;
	if (opt_len) {
		tcap_build_opts(tc, opt, mp->b_wptr);
		mp->b_wptr += opt_len;
	}
	mp->b_cont = dp;
	printd(("%s: %p: <- TR_END_IND\n", DRV_NAME, tc));
	put(tc->oq, mp);
	return (QR_ABSORBED);
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      ebusy:
	err = -EBUSY;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  TR_ABORT_IND
 *  -----------------------------------------------------------------
 */
static int
tr_abort_ind(struct tc *tc, queue_t *q, t_uscalar_t cause, t_uscalar_t orig,
	     struct tcap_opts *opt, t_uscalar_t tid, t_uscalar_t cid)
{
	int err;
	mblk_t *mp;
	struct TR_abort_ind *p;
	size_t opt_len = tcap_opts_size(tc, opt);
	size_t msg_len = sizeof(*p) + opt_len;

	if (!canput(tc->oq))
		goto ebusy;
	if (!(mp = mi_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->PRIM_type = TR_ABORT_IND;
	p->ABORT_cause = cause;
	p->ORIGINATOR = orig;
	p->CORR_id = cid;
	p->TRANS_id = tid;
	p->OPT_length = opt_len;
	p->OPT_offset = opt_len ? sizeof(*p) : 0;
	if (opt_len) {
		tcap_build_opts(tc, opt, mp->b_wptr);
		mp->b_wptr += opt_len;
	}
	printd(("%s: %p: <- TR_ABORT_IND\n", DRV_NAME, tc));
	put(tc->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      ebusy:
	err = -EBUSY;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  TR_NOTICE_IND
 *  -----------------------------------------------------------------
 */
static inline int
tr_notice_ind(struct tc *tc, queue_t *q, t_uscalar_t cause, t_uscalar_t tid, t_uscalar_t cid)
{
	int err;
	mblk_t *mp;
	struct TR_notice_ind *p;
	size_t msg_len = sizeof(*p);

	if (!canput(tc->oq))
		goto ebusy;
	if (!(mp = mi_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->PRIM_type = TR_NOTICE_IND;
	p->REPORT_cause = cause;
	p->CORR_id = cid;
	p->TRANS_id = tid;
	printd(("%s: %p: <- TR_NOTICE_IND\n", DRV_NAME, tc));
	put(tc->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      ebusy:
	err = -EBUSY;
	rare();
	goto error;
      error:
	return (err);
}

/* 
 *  TPI Primitives
 *  -------------------------------------------------------------------------
 */
#ifndef PAD4
#define PAD4(__x) (((__x)+3)&~(3))
#endif
/* 
 *  T_CONN_IND          11 - connection indication
 *  -----------------------------------------------------------------
 *  We get the information from the CR message in the connection indication.  We queue the CR message (complete with
 *  decode parameter block) itself as the connection indication.  The sequence number is really just a pointer to
 *  the first mblk_t in the received CR message.
 */
static inline int
t_conn_ind(struct tc *tc, queue_t *q, mblk_t *cp)
{
	mblk_t *mp;
	struct tr_msg *m = (typeof(m)) cp->b_rptr;
	struct T_conn_ind *p;
	struct t_opthdr *oh;
	struct sccp_addr *src = &m->sc.orig;
	size_t src_len = sizeof(*src) + src->alen;
	size_t opt_len = sizeof(*oh) + sizeof(t_scalar_t);
	size_t msg_len = sizeof(*p) + PAD4(src_len) + opt_len;

	if ((1 << tcap_get_t_state(tc)) & ~(TSF_IDLE | TSF_WRES_CIND))
		goto efault;
	if (bufq_length(&tc->conq) >= tc->conind)
		goto erestart;
	if (!canput(tc->oq))
		goto ebusy;
	if (!(mp = mi_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->PRIM_type = T_CONN_IND;
	p->SRC_length = src_len;
	p->SRC_offset = src_len ? sizeof(*p) : 0;
	p->OPT_length = opt_len;
	p->OPT_offset = opt_len ? sizeof(*p) + PAD4(src_len) : 0;
	p->SEQ_number = (t_uscalar_t) (long) cp;
	if (src_len) {
		bcopy(src, mp->b_wptr, src_len);
		mp->b_wptr += PAD4(src_len);
	}
	oh = (typeof(oh)) mp->b_wptr;
	mp->b_wptr += sizeof(*oh);
	oh->len = opt_len;
	oh->level = T_SS7_SCCP;
	oh->name = T_SCCP_PCLASS;
	oh->status = T_SUCCESS;
	*(t_uscalar_t *) mp->b_wptr = m->sc.pcl;
	mp->b_wptr += sizeof(t_uscalar_t);
	bufq_queue(&tc->conq, cp);
	tcap_set_t_state(tc, TS_WRES_CIND);
	put(tc->oq, mp);
	return (0);
      enobufs:
	rare();
	return (-ENOBUFS);
      ebusy:
	rare();
	return (-EBUSY);
      erestart:
	ptrace(("%s: %p: PROTO: too many connection indications\n", DRV_NAME, tc));
	return (-ERESTART);
      efault:
	pswerr(("%s: %p: SWERR: unexpected indication for state %u\n", DRV_NAME, tc,
		tcap_get_t_state(tc)));
	return (-EFAULT);
}

/* 
 *  T_CONN_CON          12 - connection confirmation
 *  -----------------------------------------------------------------
 *  The only options with end-to-end significance that are negotiated is the protocol class.
 */
static inline int
t_conn_con(struct tc *tc, queue_t *q, struct sccp_addr *res, t_uscalar_t pcls, mblk_t *dp)
{
	int err;
	mblk_t *mp;
	struct T_conn_con *p;
	struct t_opthdr *oh;
	size_t res_len = res ? sizeof(*res) + res->alen : 0;
	size_t opt_len = sizeof(*oh) + sizeof(t_scalar_t);
	size_t msg_len = sizeof(*p) + PAD4(res_len) + opt_len;

	if (tcap_get_t_state(tc) != TS_WCON_CREQ)
		goto efault;
	if (!canput(tc->oq))
		goto ebusy;
	if (!(mp = mi_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	mp->b_band = 1;		/* expedite */
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->PRIM_type = T_CONN_CON;
	p->RES_length = res_len;
	p->RES_offset = res_len ? sizeof(*p) : 0;
	p->OPT_length = opt_len;
	p->OPT_offset = opt_len ? sizeof(*p) + PAD4(res_len) : 0;
	if (res_len) {
		bcopy(res, mp->b_wptr, res_len);
		mp->b_wptr += PAD4(res_len);
	}
	oh = (typeof(oh)) mp->b_wptr;
	mp->b_wptr += sizeof(*oh);
	oh->len = opt_len;
	oh->level = T_SS7_SCCP;
	oh->name = T_SCCP_PCLASS;
	oh->status = T_SUCCESS;
	*(t_uscalar_t *) mp->b_wptr = pcls;
	mp->b_wptr += sizeof(t_uscalar_t);
	tcap_set_t_state(tc, TS_DATA_XFER);
	put(tc->oq, mp);
	return (0);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: no buffers\n", DRV_NAME, tc));
	goto error;
      ebusy:
	err = -EBUSY;
	ptrace(("%s: %p: ERROR: flow controlled\n", DRV_NAME, tc));
	goto error;
      efault:
	err = -EFAULT;
	pswerr(("%s: %p: SWERR: unexpected indication for state %u\n", DRV_NAME, tc,
		tcap_get_t_state(tc)));
	goto error;
      error:
	return (err);
}

/* 
 *  T_DISCON_IND        13 - disconnect indication
 *  -----------------------------------------------------------------
 *  We use the address of the mblk_t that contains the CR message as a SEQ_number for connection indications that
 *  are rejected with a disconnect indication as well.  We can use this to directly address the mblk in the
 *  connection indication bufq.
 */
static inline int
t_discon_ind(struct tc *tc, queue_t *q, t_uscalar_t reason, mblk_t *seq, mblk_t *dp)
{
	int err;
	mblk_t *mp;
	struct T_discon_ind *p;
	size_t msg_len = sizeof(*p);

	if ((1 << tc->
	     state) & ~(TSF_WCON_CREQ | TSF_WRES_CIND | TSF_DATA_XFER | TSF_WIND_ORDREL |
			TSF_WREQ_ORDREL))
		goto efault;
	if (!canput(tc->oq))
		goto ebusy;
	if (!(mp = mi_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->PRIM_type = T_DISCON_IND;
	p->DISCON_reason = reason;
	p->SEQ_number = (t_uscalar_t) (long) seq;
	if (seq) {
		bufq_unlink(&tc->conq, seq);
		freemsg(seq);
	}
	if (!bufq_length(&tc->conq))
		tcap_set_t_state(tc, TS_IDLE);
	else
		tcap_set_t_state(tc, TS_WRES_CIND);
	mp->b_cont = dp;
	put(tc->oq, mp);
	return (0);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: no buffers\n", DRV_NAME, tc));
	goto error;
      ebusy:
	err = -EBUSY;
	ptrace(("%s: %p: ERROR: flow controlled\n", DRV_NAME, tc));
	goto error;
      efault:
	err = -EFAULT;
	pswerr(("%s: %p: SWERR: unexpected indication for state %u\n", DRV_NAME, tc,
		tcap_get_t_state(tc)));
	goto error;
      error:
	return (err);
}

/* 
 *  T_DATA_IND          14 - data indication
 *  -----------------------------------------------------------------
 */
static inline int
t_data_ind(struct tc *tc, queue_t *q, t_uscalar_t more, mblk_t *dp)
{
	int err;
	mblk_t *mp;
	struct T_data_ind *p;
	size_t msg_len = sizeof(*p);

	if ((1 << tcap_get_t_state(tc)) & ~(TSF_DATA_XFER | TSF_WIND_ORDREL))
		goto efault;
	if (!canput(tc->oq))
		goto ebusy;
	if (!(mp = mi_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->PRIM_type = T_DATA_IND;
	p->MORE_flag = more;
	mp->b_cont = dp;
	put(tc->oq, mp);
	return (0);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: no buffers\n", DRV_NAME, tc));
	goto error;
      ebusy:
	err = -EBUSY;
	ptrace(("%s: %p: ERROR: flow controlled\n", DRV_NAME, tc));
	goto error;
      efault:
	err = -EFAULT;
	pswerr(("%s: %p: SWERR: unexpected indication for state %u\n", DRV_NAME, tc,
		tcap_get_t_state(tc)));
	goto error;
      error:
	return (err);
}

/* 
 *  T_EXDATA_IND        15 - expedited data indication
 *  -----------------------------------------------------------------
 */
static inline int
t_exdata_ind(struct tc *tc, queue_t *q, t_uscalar_t more, mblk_t *dp)
{
	int err;
	mblk_t *mp;
	struct T_exdata_ind *p;
	size_t msg_len = sizeof(*p);

	if ((1 << tcap_get_t_state(tc)) & ~(TSF_DATA_XFER | TSF_WIND_ORDREL))
		goto efault;
	if (!canput(tc->oq))
		goto ebusy;
	if (!(mp = mi_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	mp->b_band = 1;		/* expedite */
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->PRIM_type = T_EXDATA_IND;
	p->MORE_flag = more;
	mp->b_cont = dp;
	put(tc->oq, mp);
	return (0);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: no buffers\n", DRV_NAME, tc));
	goto error;
      ebusy:
	err = -EBUSY;
	ptrace(("%s: %p: ERROR: flow controlled\n", DRV_NAME, tc));
	goto error;
      efault:
	err = -EFAULT;
	pswerr(("%s: %p: SWERR: unexpected indication for state %u\n", DRV_NAME, tc,
		tcap_get_t_state(tc)));
	goto error;
      error:
	return (err);
}

/* 
 *  T_INFO_ACK          16 - information acknowledgement
 *  -----------------------------------------------------------------
 */
static inline int
t_info_ack(struct tc *tc, queue_t *q)
{
	int err;
	mblk_t *mp;
	struct T_info_ack *p;
	size_t msg_len = sizeof(*p);

	if (!(mp = mi_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	{
		t_uscalar_t serv = tc->pclass < 2 ? T_CLTS : T_COTS_ORD;
		t_uscalar_t etsdu = tc->pclass < 2 ? T_INVALID : tc->options.mtu;

		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		fixme(("Still some things to double-check here\n"));
		p->PRIM_type = T_INFO_ACK;
		p->TSDU_size = T_INFINITE;	/* no limit on TSDU size */
		p->ETSDU_size = etsdu;	/* no concept of ETSDU size */
		p->CDATA_size = tc->options.mtu;	/* no concept of CDATA size */
		p->DDATA_size = tc->options.mtu;	/* no concept of DDATA size */
		p->ADDR_size = sizeof(struct sccp_addr);	/* no limit on ADDR size */
		p->OPT_size = T_INFINITE;	/* no limit on OPTIONS size */
		p->TIDU_size = T_INFINITE;	/* no limit on TIDU size */
		p->SERV_type = serv;	/* COTS or CLTS */
		p->CURRENT_state = tcap_get_t_state(tc);
		p->PROVIDER_flag = XPG4_1 & ~T_SNDZERO;
		put(tc->oq, mp);
		return (0);
	}
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, tc));
	goto error;
      error:
	return (err);
}

/* 
 *  T_BIND_ACK          17 - bind acknowledgement
 *  -----------------------------------------------------------------
 */
static inline int
t_bind_ack(struct tc *tc, queue_t *q, struct sccp_addr *add)
{
	int err;
	mblk_t *mp;
	struct T_bind_ack *p;
	size_t add_len = add ? sizeof(*add) : 0;
	size_t msg_len = sizeof(*p) + add_len;

	if (tcap_get_t_state(tc) != TS_WACK_BREQ)
		goto efault;
	if (!(mp = mi_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->PRIM_type = T_BIND_ACK;
	p->ADDR_length = add_len;
	p->ADDR_offset = add_len ? sizeof(*p) : 0;
	p->CONIND_number = tc->conind;
	if (add_len) {
		bcopy(add, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
	}
	tcap_set_t_state(tc, TS_IDLE);
	put(tc->oq, mp);
	return (0);
      efault:
	err = -EFAULT;
	pswerr(("%s: %p: SWERR: unexpected indication for state %u\n", DRV_NAME, tc,
		tcap_get_t_state(tc)));
	goto error;
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, tc));
	goto error;
      error:
	return (err);
}

/* 
 *  T_ERROR_ACK         18 - error acknowledgement
 *  -----------------------------------------------------------------
 */
static int
t_error_ack(struct tc *tc, queue_t *q, const t_uscalar_t prim, t_scalar_t error)
{
	int err = error;
	mblk_t *mp;
	struct T_error_ack *p;
	size_t msg_len = sizeof(*p);

	switch (error) {
	case -EBUSY:
	case -EAGAIN:
	case -ENOMEM:
	case -ENOBUFS:
		seldom();
		goto error;
	case 0:
		never();
		goto error;
	}
	if (!(mp = mi_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->PRIM_type = T_ERROR_ACK;
	p->ERROR_prim = prim;
	p->TLI_error = error < 0 ? TSYSERR : error;
	p->UNIX_error = error < 0 ? -error : 0;
	switch (tcap_get_t_state(tc)) {
#ifdef TS_WACK_OPTREQ
	case TS_WACK_OPTREQ:
#endif
	case TS_WACK_UREQ:
	case TS_WACK_CREQ:
		tcap_set_t_state(tc, TS_IDLE);
		break;
	case TS_WACK_BREQ:
		tcap_set_t_state(tc, TS_UNBND);
		break;
	case TS_WACK_CRES:
		tcap_set_t_state(tc, TS_WRES_CIND);
		break;
	case TS_WACK_DREQ6:
		tcap_set_t_state(tc, TS_WCON_CREQ);
		break;
	case TS_WACK_DREQ7:
		tcap_set_t_state(tc, TS_WRES_CIND);
		break;
	case TS_WACK_DREQ9:
		tcap_set_t_state(tc, TS_DATA_XFER);
		break;
	case TS_WACK_DREQ10:
		tcap_set_t_state(tc, TS_WIND_ORDREL);
		break;
	case TS_WACK_DREQ11:
		tcap_set_t_state(tc, TS_WREQ_ORDREL);
		break;
		/* Note: if we are not in a WACK state we simply do not change state.  This occurs
		   normally when we send TOUTSTATE or TNOTSUPPORT or are responding to a
		   T_OPTMGMT_REQ in other then TS_IDLE state. */
	}
	put(tc->oq, mp);
	return (0);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, tc));
	goto error;
      error:
	return (err);
}

/* 
 *  T_OK_ACK            19 - success acknowledgement
 *  -----------------------------------------------------------------
 */
static inline int
t_ok_ack(struct tc *tc, queue_t *q, t_uscalar_t prim, t_uscalar_t seq, t_uscalar_t tok)
{
	int err;
	mblk_t *mp;
	struct T_ok_ack *p;
	size_t msg_len = sizeof(*p);

	if (!(mp = mi_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->PRIM_type = T_OK_ACK;
	p->CORRECT_prim = prim;
	switch (tcap_get_t_state(tc)) {
	case TS_WACK_CREQ:
		tcap_set_t_state(tc, TS_WCON_CREQ);
		break;
	case TS_WACK_UREQ:
		tcap_set_t_state(tc, TS_UNBND);
		break;
	case TS_WACK_CRES:
	{
		queue_t *aq = (queue_t *) (long) tok;	/* FIXME: look up instead */
		struct tc *ap = TC_PRIV(aq);

		if (ap) {
			ap->i_state = TS_DATA_XFER;
//                      tcap_cleanup_read(q);
//                      tcap_transmit_wakeup(q);
		}
		if (seq) {
			bufq_unlink(&tc->conq, (mblk_t *) (long) seq);	/* FIXME: look up instead */
			freemsg((mblk_t *) (long) seq);	/* FIXME: look up instead */
		}
		if (aq != tc->oq) {
			if (bufq_length(&tc->conq))
				tcap_set_t_state(tc, TS_WRES_CIND);
			else
				tcap_set_t_state(tc, TS_IDLE);
		}
		break;
	}
	case TS_WACK_DREQ7:
		if (seq)
			bufq_unlink(&tc->conq, (mblk_t *) (long) seq);	/* FIXME: look up instead */
	case TS_WACK_DREQ6:
	case TS_WACK_DREQ9:
	case TS_WACK_DREQ10:
	case TS_WACK_DREQ11:
		if (bufq_length(&tc->conq))
			tcap_set_t_state(tc, TS_WRES_CIND);
		else
			tcap_set_t_state(tc, TS_IDLE);
		break;
		/* Note: if we are not in a WACK state we simply do not change state.  This occurs
		   normally when we are responding to a T_OPTMGMT_REQ in other than the TS_IDLE
		   state. */
	}
	put(tc->oq, mp);
	return (0);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, tc));
	goto error;
      error:
	return (err);
}

/* 
 *  T_OPTMGMT_ACK       22 - options management acknowledgement
 *  -----------------------------------------------------------------
 */
static inline int
t_optmgmt_ack(struct tc *tc, queue_t *q, t_uscalar_t flags, struct tcap_opts *ops)
{
	int err;
	mblk_t *mp;
	struct T_optmgmt_ack *p;
	size_t opt_len = tcap_opts_size(tc, ops);
	size_t msg_len = sizeof(*p) + opt_len;

	if (!(mp = mi_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->PRIM_type = T_OPTMGMT_ACK;
	p->OPT_length = opt_len;
	p->OPT_offset = opt_len ? sizeof(*p) : 0;
	p->MGMT_flags = flags;
	tcap_build_opts(tc, ops, mp->b_wptr);
	mp->b_wptr += opt_len;
#ifdef TS_WACK_OPTREQ
	if (tcap_get_t_state(tc) == TS_WACK_OPTREQ)
		tcap_set_t_state(tc, TS_IDLE);
#endif
	put(tc->oq, mp);
	return (0);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, tc));
	goto error;
      error:
	return (err);
}

/* 
 *  T_ORDREL_IND        23 - orderly release indication
 *  -----------------------------------------------------------------
 */
static inline int
t_ordrel_ind(struct tc *tc, queue_t *q)
{
	int err;
	mblk_t *mp;
	struct T_ordrel_ind *p;
	size_t msg_len = sizeof(*p);

	if ((1 << tcap_get_t_state(tc)) & ~(TSF_DATA_XFER | TSF_WIND_ORDREL))
		goto efault;
	if (!canput(tc->oq))
		goto ebusy;
	if (!(mp = mi_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->PRIM_type = T_ORDREL_IND;
	switch (tcap_get_t_state(tc)) {
	case TS_DATA_XFER:
		tcap_set_t_state(tc, TS_WREQ_ORDREL);
		break;
	case TS_WIND_ORDREL:
		tcap_set_t_state(tc, TS_IDLE);
		break;
	}
	put(tc->oq, mp);
	return (0);
      efault:
	err = -EFAULT;
	pswerr(("%s: %p: SWERR: unexpected indication for state %u\n", DRV_NAME, tc,
		tcap_get_t_state(tc)));
	goto error;
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, tc));
	goto error;
      ebusy:
	err = -EBUSY;
	ptrace(("%s: %p: ERROR: flow controlled\n", DRV_NAME, tc));
	goto error;
      error:
	return (err);
}

/* 
 *  T_ADDR_ACK          27 - address acknowledgement
 *  -----------------------------------------------------------------
 */
static inline int
t_addr_ack(struct tc *tc, queue_t *q, struct sccp_addr *loc, struct sccp_addr *rem)
{
	int err;
	mblk_t *mp;
	struct T_addr_ack *p;
	size_t loc_len = loc ? sizeof(*loc) : 0;
	size_t rem_len = rem ? sizeof(*rem) : 0;
	size_t msg_len = sizeof(*p) + loc_len + rem_len;

	if (!(mp = mi_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->PRIM_type = T_ADDR_ACK;
	p->LOCADDR_length = loc_len;
	p->LOCADDR_offset = loc_len ? sizeof(*p) : 0;
	p->REMADDR_length = rem_len;
	p->REMADDR_offset = rem_len ? sizeof(*p) + loc_len : 0;
	if (loc_len) {
		bcopy(loc, mp->b_wptr, loc_len);
		mp->b_wptr += loc_len;
	}
	if (rem_len) {
		bcopy(rem, mp->b_wptr, rem_len);
		mp->b_wptr += rem_len;
	}
	put(tc->oq, mp);
	return (0);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, tc));
	goto error;
      error:
	return (err);
}

#if 0
/* 
 *  T_CAPABILITY_ACK    ?? - protocol capability ack
 *  -----------------------------------------------------------------
 */
static int
t_capability_ack(struct tc *tc, queue_t *q, t_uscalar_t caps)
{
	int err;
	mblk_t *mp;
	struct T_capability_ack *p;
	size_t msg_len = sizeof(*p);
	t_uscalar_t caps = (acceptor ? TC1_ACCEPTOR_ID : 0) | (info ? TC1_INFO : 0);

	if ((mp = mi_allocb(q, msg_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((struct T_capability_ack *) mp->b_wptr)++;
		p->PRIM_type = T_CAPABILITY_ACK;
		p->CAP_bits1 = caps;
		p->ACCEPTOR_id = (caps & TC1_ACCEPTOR_ID) ? (t_uscalar_t) tc->oq : 0;
		if (caps & TC1_INFO) {
			p->INFO_ack.PRIM_type = T_INFO_ACK;
			p->INFO_ack.TSDU_size = tc->tsdu;
			p->INFO_ack.ETSDU_size = tc->etsdu;
			p->INFO_ack.CDATA_size = tc->cdata;
			p->INFO_ack.DDATA_size = tc->ddata;
			p->INFO_ack.ADDR_size = tc->addlen;
			p->INFO_ack.OPT_size = tc->optlen;
			p->INFO_ack.TIDU_size = tc->tidu;
			p->INFO_ack.SERV_type = tc->stype;
			p->INFO_ack.CURRENT_state = tcap_get_t_state(tc);
			p->INFO_ack.PROVIDER_flag = tc->ptype;
		} else
			bzero(&p->INFO_ack, sizeof(p->INFO_ack));
		put(tc->oq, mp);
		return (0);
	} else {
		err = -ENOBUFS;
		ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, tc));
	}
	return (err);
}
#endif

/* 
 *  T_UNITDATA_IND      20 - Unitdata indication
 *  -----------------------------------------------------------------
 *  Note: If we cannot deliver the destination address in the option header or somewhere, it will not be possible to
 *  bind to multiple alias addresses, but will only permit us to bind to a single alias address.  This might or
 *  might not be a problem to the user.
 */
static inline int
t_unitdata_ind(struct tc *tc, queue_t *q, struct sccp_addr *src, struct tcap_opts *opt, mblk_t *dp)
{
	int err;
	mblk_t *mp;
	struct t_opthdr *oh;
	const size_t olen = sizeof(*oh) + sizeof(t_scalar_t);
	struct T_unitdata_ind *p;
	size_t src_len = src ? sizeof(*src) : 0;
	size_t opt_len =
	    (opt->seq ? olen : 0) + (opt->pri ? olen : 0) + (opt->pcl ? olen : 0) +
	    (opt->imp ? olen : 0) + (opt->ret ? olen : 0);
	size_t msg_len = sizeof(*p) + src_len + opt_len;

	if (!canput(tc->oq))
		goto ebusy;
	if (!(mp = mi_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->PRIM_type = T_UNITDATA_IND;
	p->SRC_length = src_len;
	p->SRC_offset = src_len ? sizeof(*p) : 0;
	p->OPT_length = opt_len;
	p->OPT_offset = opt_len ? sizeof(*p) + src_len : 0;
	if (src_len) {
		bcopy(src, mp->b_wptr, src_len);
		mp->b_wptr += src_len;
	}
	if (opt_len) {
		if (opt->seq) {
			oh = (typeof(oh)) mp->b_wptr;
			mp->b_wptr += sizeof(*oh);
			oh->len = olen;
			oh->level = T_SS7_SCCP;
			oh->name = T_SCCP_SEQ_CTRL;
			oh->status = T_SUCCESS;
			*(t_uscalar_t *) mp->b_wptr = *opt->seq;
			mp->b_wptr += sizeof(t_uscalar_t);
		}
		if (opt->pri) {
			oh = (typeof(oh)) mp->b_wptr;
			mp->b_wptr += sizeof(*oh);
			oh->len = olen;
			oh->level = T_SS7_SCCP;
			oh->name = T_SCCP_PRIORITY;
			oh->status = T_SUCCESS;
			*(t_uscalar_t *) mp->b_wptr = *opt->pri;
			mp->b_wptr += sizeof(t_uscalar_t);
		}
		if (opt->pcl) {
			oh = (typeof(oh)) mp->b_wptr;
			mp->b_wptr += sizeof(*oh);
			oh->len = olen;
			oh->level = T_SS7_SCCP;
			oh->name = T_SCCP_PCLASS;
			oh->status = T_SUCCESS;
			*(t_uscalar_t *) mp->b_wptr = *opt->pcl;
			mp->b_wptr += sizeof(t_uscalar_t);
		}
		if (opt->imp) {
			oh = (typeof(oh)) mp->b_wptr;
			mp->b_wptr += sizeof(*oh);
			oh->len = olen;
			oh->level = T_SS7_SCCP;
			oh->name = T_SCCP_IMPORTANCE;
			oh->status = T_SUCCESS;
			*(t_uscalar_t *) mp->b_wptr = *opt->imp;
			mp->b_wptr += sizeof(t_uscalar_t);
		}
		if (opt->ret) {
			oh = (typeof(oh)) mp->b_wptr;
			mp->b_wptr += sizeof(*oh);
			oh->len = olen;
			oh->level = T_SS7_SCCP;
			oh->name = T_SCCP_RET_ERROR;
			oh->status = T_SUCCESS;
			*(t_uscalar_t *) mp->b_wptr = *opt->ret;
			mp->b_wptr += sizeof(t_uscalar_t);
		}
	}
	put(tc->oq, mp);
	return (0);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, tc));
	goto error;
      ebusy:
	err = -EBUSY;
	ptrace(("%s: %p: ERROR: Flow controlled\n", DRV_NAME, tc));
	goto error;
      error:
	return (err);
}

/* 
 *  T_UDERROR_IND       21 - Unitdata error indication
 *  -----------------------------------------------------------------
 *  This primitive indicates to the transport user that a datagram with the specified destination address and
 *  options produced an error.
 */
static inline int
t_uderror_ind(struct tc *tc, queue_t *q, t_uscalar_t etype, struct sccp_addr *dst,
	      t_uscalar_t *seq, t_uscalar_t *pri, t_uscalar_t *pcl, t_uscalar_t *imp,
	      t_uscalar_t *ret, mblk_t *dp)
{
	int err;
	mblk_t *mp;
	struct t_opthdr *oh;
	const size_t olen = sizeof(*oh) + sizeof(t_scalar_t);
	struct T_uderror_ind *p;
	size_t dst_len = dst ? sizeof(*dst) : 0;
	size_t opt_len =
	    (seq ? olen : 0) + (pri ? olen : 0) + (pcl ? olen : 0) + (imp ? olen : 0) +
	    (ret ? olen : 0);
	size_t msg_len = sizeof(*p) + dst_len;

	if (!canput(tc->oq))
		goto ebusy;
	if (!(mp = mi_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	mp->b_band = 2;		/* XXX move ahead of data indications */
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->PRIM_type = T_UDERROR_IND;
	p->DEST_length = dst_len;
	p->DEST_offset = dst_len ? sizeof(*p) : 0;
	p->OPT_length = opt_len;
	p->OPT_offset = opt_len ? sizeof(*p) + dst_len : 0;
	p->ERROR_type = etype;
	if (dst_len) {
		bcopy(dst, mp->b_wptr, dst_len);
		mp->b_wptr += dst_len;
	}
	if (opt_len) {
		if (seq) {
			oh = (typeof(oh)) mp->b_wptr;
			mp->b_wptr += sizeof(*oh);
			oh->len = olen;
			oh->level = T_SS7_SCCP;
			oh->name = T_SCCP_SEQ_CTRL;
			oh->status = T_SUCCESS;
			*(t_uscalar_t *) mp->b_wptr = *seq;
			mp->b_wptr += sizeof(t_uscalar_t);
		}
		if (pri) {
			oh = (typeof(oh)) mp->b_wptr;
			mp->b_wptr += sizeof(*oh);
			oh->len = olen;
			oh->level = T_SS7_SCCP;
			oh->name = T_SCCP_PRIORITY;
			oh->status = T_SUCCESS;
			*(t_uscalar_t *) mp->b_wptr = *pri;
			mp->b_wptr += sizeof(t_uscalar_t);
		}
		if (pcl) {
			oh = (typeof(oh)) mp->b_wptr;
			mp->b_wptr += sizeof(*oh);
			oh->len = olen;
			oh->level = T_SS7_SCCP;
			oh->name = T_SCCP_PCLASS;
			oh->status = T_SUCCESS;
			*(t_uscalar_t *) mp->b_wptr = *pcl;
			mp->b_wptr += sizeof(t_uscalar_t);
		}
		if (imp) {
			oh = (typeof(oh)) mp->b_wptr;
			mp->b_wptr += sizeof(*oh);
			oh->len = olen;
			oh->level = T_SS7_SCCP;
			oh->name = T_SCCP_IMPORTANCE;
			oh->status = T_SUCCESS;
			*(t_uscalar_t *) mp->b_wptr = *imp;
			mp->b_wptr += sizeof(t_uscalar_t);
		}
		if (ret) {
			oh = (typeof(oh)) mp->b_wptr;
			mp->b_wptr += sizeof(*oh);
			oh->len = olen;
			oh->level = T_SS7_SCCP;
			oh->name = T_SCCP_RET_ERROR;
			oh->status = T_SUCCESS;
			*(t_uscalar_t *) mp->b_wptr = *ret;
			mp->b_wptr += sizeof(t_uscalar_t);
		}
	}
	put(tc->oq, mp);
	return (0);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, tc));
	goto error;
      ebusy:
	err = -EBUSY;
	ptrace(("%s: %p: ERROR: Flow controlled\n", DRV_NAME, tc));
	goto error;
      error:
	return (err);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  Primitives sent downstream (to SCCP)
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  N_CONN_REQ
 *  -----------------------------------
 */
static inline int
n_conn_req(struct sc *sc, queue_t *q, struct sccp_addr *dst, t_uscalar_t flags,
	   N_qos_sel_conn_sccp_t *qos, mblk_t *dp)
{
	int err;
	mblk_t *mp;
	N_conn_req_t *p;
	size_t dst_len = dst ? sizeof(*dst) + dst->alen : 0;
	size_t qos_len = qos ? sizeof(*qos) : 0;
	size_t msg_len = sizeof(*p) + dst_len + qos_len;

	if ((1 << sccp_get_i_state(sc)) & ~(NSF_IDLE))
		goto efault;
	if (!canput(sc->oq))
		goto ebusy;
	if (!(mp = mi_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->PRIM_type = N_CONN_REQ;
	p->DEST_length = dst_len;
	p->DEST_offset = dst_len ? sizeof(*p) : 0;
	p->CONN_flags = flags;
	p->QOS_length = qos_len;
	p->QOS_offset = qos_len ? sizeof(*p) + dst_len : 0;
	if (dst_len) {
		bcopy(dst, mp->b_wptr, dst_len);
		mp->b_wptr += dst_len;
	}
	if (qos_len) {
		bcopy(qos, mp->b_wptr, qos_len);
		mp->b_wptr += qos_len;
	}
	mp->b_cont = dp;
	sccp_set_i_state(sc, NS_WCON_CREQ);
	printd(("%s: %p: N_CONN_REQ ->\n", DRV_NAME, sc));
	put(sc->oq, mp);
	return (QR_ABSORBED);
      efault:
	err = -EFAULT;
	swerr();
	goto error;
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      ebusy:
	err = -EBUSY;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  N_CONN_RES
 *  -----------------------------------
 */
static inline int
n_conn_res(struct sc *sc, queue_t *q, t_uscalar_t tok, struct sccp_addr *res, t_uscalar_t seq,
	   t_uscalar_t flags, N_qos_sel_conn_sccp_t *qos, mblk_t *dp)
{
	int err;
	mblk_t *mp;
	N_conn_res_t *p;
	size_t res_len = res ? sizeof(*res) + res->alen : 0;
	size_t qos_len = qos ? sizeof(*qos) : 0;
	size_t msg_len = sizeof(*p) + res_len + qos_len;

	if ((1 << sccp_get_i_state(sc)) & ~(NSF_WRES_CIND))
		goto efault;
	if (!canput(sc->oq))
		goto ebusy;
	if (!(mp = mi_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->PRIM_type = N_CONN_RES;
	p->TOKEN_value = tok;
	p->RES_length = res_len;
	p->RES_offset = res_len ? sizeof(*p) : 0;
	p->SEQ_number = seq;
	p->CONN_flags = flags;
	p->QOS_length = qos_len;
	p->QOS_offset = qos_len ? sizeof(*p) + res_len : 0;
	if (res_len) {
		bcopy(res, mp->b_wptr, res_len);
		mp->b_wptr += res_len;
	}
	if (qos_len) {
		bcopy(qos, mp->b_wptr, qos_len);
		mp->b_wptr += qos_len;
	}
	mp->b_cont = dp;
	if (--sc->outcnt)
		sccp_set_i_state(sc, NS_IDLE);
	else
		sccp_set_i_state(sc, NS_WRES_CIND);
	printd(("%s: %p: N_CONN_RES ->\n", DRV_NAME, sc));
	put(sc->oq, mp);
	return (QR_ABSORBED);
      efault:
	err = -EFAULT;
	swerr();
	goto error;
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      ebusy:
	err = -EBUSY;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  N_DISCON_REQ
 *  -----------------------------------
 */
static inline int
n_discon_req(struct sc *sc, queue_t *q, t_uscalar_t reason, struct sccp_addr *res,
	     t_uscalar_t seq, mblk_t *dp)
{
	int err;
	mblk_t *mp;
	N_discon_req_t *p;
	size_t res_len = res ? sizeof(*res) + res->alen : 0;
	size_t msg_len = sizeof(*p) + res_len;

	if ((1 << sccp_get_i_state(sc)) &
	    ~(NSF_WCON_CREQ | NSF_WRES_CIND | NSF_DATA_XFER | NSF_WCON_CREQ | NSF_WRES_RIND))
		goto efault;
	if (!canput(sc->oq))
		goto ebusy;
	if (!(mp = mi_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->PRIM_type = N_DISCON_REQ;
	p->DISCON_reason = reason;
	p->RES_length = res_len;
	p->RES_offset = res_len ? sizeof(*p) : 0;
	p->SEQ_number = seq;
	if (res_len) {
		bcopy(res, mp->b_wptr, res_len);
		mp->b_wptr += res_len;
	}
	mp->b_cont = dp;
	switch (sccp_get_i_state(sc)) {
	case NS_WCON_CREQ:
		sccp_set_i_state(sc, NS_WACK_DREQ6);
		break;
	case NS_WRES_CIND:
		sccp_set_i_state(sc, NS_WACK_DREQ7);
		break;
	case NS_DATA_XFER:
		sccp_set_i_state(sc, NS_WACK_DREQ9);
		break;
	case NS_WCON_RREQ:
		sccp_set_i_state(sc, NS_WACK_DREQ10);
		break;
	case NS_WRES_RIND:
		sccp_set_i_state(sc, NS_WACK_DREQ11);
		break;
	default:
		swerr();
		break;
	}
	printd(("%s: %p: N_DISCON_REQ ->\n", DRV_NAME, sc));
	put(sc->oq, mp);
	return (QR_ABSORBED);
      efault:
	err = -EFAULT;
	swerr();
	goto error;
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      ebusy:
	err = -EBUSY;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  N_DATA_REQ
 *  -----------------------------------
 */
static inline int
n_data_req(struct sc *sc, queue_t *q, t_uscalar_t flags, mblk_t *dp)
{
	int err;
	mblk_t *mp;
	N_data_req_t *p;
	size_t msg_len = sizeof(*p);

	if ((1 << sccp_get_i_state(sc)) & ~(NSF_DATA_XFER))
		goto efault;
	if (!canput(sc->oq))
		goto ebusy;
	if (!(mp = mi_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->PRIM_type = N_DATA_REQ;
	p->DATA_xfer_flags = flags;
	mp->b_cont = dp;
	printd(("%s: %p: N_DATA_REQ ->\n", DRV_NAME, sc));
	put(sc->oq, mp);
	return (QR_ABSORBED);
      efault:
	if (sccp_get_i_state(sc) == NS_IDLE)
		goto discard;
	err = -EFAULT;
	swerr();
	goto error;
      discard:
	err = QR_DONE;
	rare();
	goto error;
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      ebusy:
	err = -EBUSY;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  N_EXDATA_REQ
 *  -----------------------------------
 */
static inline int
n_exdata_req(struct sc *sc, queue_t *q, mblk_t *dp)
{
	int err;
	mblk_t *mp;
	N_exdata_req_t *p;
	size_t msg_len = sizeof(*p);

	if ((1 << sccp_get_i_state(sc)) & ~(NSF_DATA_XFER))
		goto efault;
	if (!canput(sc->oq))
		goto ebusy;
	if (!(mp = mi_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	mp->b_band = 1;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->PRIM_type = N_EXDATA_REQ;
	mp->b_cont = dp;
	printd(("%s: %p: N_EXDATA_REQ ->\n", DRV_NAME, sc));
	put(sc->oq, mp);
	return (QR_ABSORBED);
      efault:
	if (sccp_get_i_state(sc) == NS_IDLE)
		goto discard;
	err = -EFAULT;
	swerr();
	goto error;
      discard:
	err = QR_DONE;
	rare();
	goto error;
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      ebusy:
	err = -EBUSY;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  N_INFO_REQ
 *  -----------------------------------
 */
static inline int
n_info_req(struct sc *sc, queue_t *q, mblk_t *msg)
{
	int err;
	mblk_t *mp;
	N_info_req_t *p;
	size_t msg_len = sizeof(*p);

	if (!(mp = mi_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->PRIM_type = N_INFO_REQ;
	freemsg(msg);
	printd(("%s: %p: N_INFO_REQ ->\n", DRV_NAME, sc));
	put(sc->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  N_BIND_REQ
 *  -----------------------------------
 */
static inline int
n_bind_req(struct sc *sc, queue_t *q, struct sccp_addr *add, t_uscalar_t cons,
	   t_uscalar_t flags, uchar *pro, size_t pro_len)
{
	int err;
	mblk_t *mp;
	N_bind_req_t *p;
	size_t add_len = add ? sizeof(*add) + add->alen : 0;
	size_t msg_len = sizeof(*p) + add_len + pro_len;

	if ((1 << sccp_get_i_state(sc)) & ~(NSF_UNBND))
		goto efault;
	if (!(mp = mi_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->PRIM_type = N_BIND_REQ;
	p->ADDR_length = add_len;
	p->ADDR_offset = add_len ? sizeof(*p) : 0;
	p->CONIND_number = cons;
	p->BIND_flags = flags;
	p->PROTOID_length = pro_len;
	p->PROTOID_offset = pro_len ? sizeof(*p) + add_len : 0;
	if (add_len) {
		bcopy(add, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
	}
	if (pro_len) {
		bcopy(pro, mp->b_wptr, pro_len);
		mp->b_wptr += pro_len;
	}
	sccp_set_i_state(sc, NS_WACK_BREQ);
	printd(("%s: %p: N_BIND_REQ ->\n", DRV_NAME, sc));
	put(sc->oq, mp);
	return (QR_DONE);
      efault:
	err = -EFAULT;
	swerr();
	goto error;
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  N_UNBIND_REQ
 *  -----------------------------------
 */
static inline int
n_unbind_req(struct sc *sc, queue_t *q)
{
	int err;
	mblk_t *mp;
	N_unbind_req_t *p;
	size_t msg_len = sizeof(*p);

	if ((1 << sccp_get_i_state(sc)) & ~(NSF_IDLE))
		goto efault;
	if (!(mp = mi_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->PRIM_type = N_UNBIND_REQ;
	sccp_set_i_state(sc, NS_WACK_UREQ);
	printd(("%s: %p: N_UNBIND_REQ ->\n", DRV_NAME, sc));
	put(sc->oq, mp);
	return (QR_DONE);
      efault:
	err = -EFAULT;
	swerr();
	goto error;
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  N_UNITDATA_REQ
 *  -----------------------------------
 */
static inline int
n_unitdata_req(struct sc *sc, queue_t *q, struct sccp_addr *dst, mblk_t *dp)
{
	int err;
	mblk_t *mp;
	N_unitdata_req_t *p;
	size_t dst_len = dst ? sizeof(*dst) + dst->alen : 0;
	size_t msg_len = sizeof(*p) + dst_len;

	if ((1 << sccp_get_i_state(sc)) & ~(NSF_IDLE))
		goto efault;
	if (!canput(sc->oq))
		goto ebusy;
	if (!(mp = mi_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->PRIM_type = N_UNITDATA_REQ;
	p->DEST_length = dst_len;
	p->DEST_offset = dst_len ? sizeof(*p) : 0;
	p->RESERVED_field[0] = 0;
	p->RESERVED_field[1] = 0;
	if (dst_len) {
		bcopy(dst, mp->b_wptr, dst_len);
		mp->b_wptr += dst_len;
	}
	mp->b_cont = dp;
	printd(("%s: %p: N_UNITDATA_REQ ->\n", DRV_NAME, sc));
	put(sc->oq, mp);
	return (QR_ABSORBED);
      efault:
	err = -EFAULT;
	swerr();
	goto error;
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      ebusy:
	err = -EBUSY;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  N_OPTMGMT_REQ
 *  -----------------------------------
 */
static inline int
n_optmgmt_req(struct sc *sc, queue_t *q, N_qos_sel_info_sccp_t *qos, t_uscalar_t flags)
{
	int err;
	mblk_t *mp;
	N_optmgmt_req_t *p;
	size_t qos_len = qos ? sizeof(*qos) : 0;
	size_t msg_len = sizeof(*p) + qos_len;

	if (!canput(sc->oq))
		goto ebusy;
	if (!(mp = mi_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->PRIM_type = N_OPTMGMT_REQ;
	p->QOS_length = qos_len;
	p->QOS_offset = qos_len ? sizeof(*p) : 0;
	p->OPTMGMT_flags = flags;
	if (sccp_get_i_state(sc) == NS_IDLE)
		sccp_set_i_state(sc, NS_WACK_OPTREQ);
	printd(("%s: %p: N_OPTMGMT_REQ ->\n", DRV_NAME, sc));
	put(sc->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      ebusy:
	err = -EBUSY;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  N_DATACK_REQ
 *  -----------------------------------
 */
static inline int
n_datack_req(struct sc *sc, queue_t *q)
{
	int err;
	mblk_t *mp;
	N_datack_req_t *p;
	size_t msg_len = sizeof(*p);

	if ((1 << sccp_get_i_state(sc)) & ~(NSF_DATA_XFER))
		goto efault;
	if (!canput(sc->oq))
		goto ebusy;
	if (!(mp = mi_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->PRIM_type = N_DATACK_REQ;
	printd(("%s: %p: N_DATACK_REQ ->\n", DRV_NAME, sc));
	put(sc->oq, mp);
	return (QR_DONE);
      efault:
	if (sccp_get_i_state(sc) == NS_IDLE)
		goto discard;
	err = -EFAULT;
	swerr();
	goto error;
      discard:
	err = QR_DONE;
	rare();
	goto error;
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      ebusy:
	err = -EBUSY;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  N_RESET_REQ
 *  -----------------------------------
 */
static inline int
n_reset_req(struct sc *sc, queue_t *q, t_uscalar_t reason)
{
	int err;
	mblk_t *mp;
	N_reset_req_t *p;
	size_t msg_len = sizeof(*p);

	if ((1 << sccp_get_i_state(sc)) & ~(NSF_DATA_XFER))
		goto efault;
	if (!canput(sc->oq))
		goto ebusy;
	if (!(mp = mi_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->PRIM_type = N_RESET_REQ;
	p->RESET_reason = reason;
	sccp_set_i_state(sc, NS_WCON_RREQ);
	printd(("%s: %p: N_RESET_REQ ->\n", DRV_NAME, sc));
	put(sc->oq, mp);
	return (QR_DONE);
      efault:
	err = -EFAULT;
	swerr();
	goto error;
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      ebusy:
	err = -EBUSY;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  N_RESET_RES
 *  -----------------------------------
 */
static inline int
n_reset_res(struct sc *sc, queue_t *q)
{
	int err;
	mblk_t *mp;
	N_reset_res_t *p;
	size_t msg_len = sizeof(*p);

	if ((1 << sccp_get_i_state(sc)) & ~(NSF_DATA_XFER))
		goto efault;
	if (!canput(sc->oq))
		goto ebusy;
	if (!(mp = mi_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->PRIM_type = N_RESET_RES;
	sccp_set_i_state(sc, NS_DATA_XFER);
	printd(("%s: %p: N_RESET_RES ->\n", DRV_NAME, sc));
	put(sc->oq, mp);
	return (QR_DONE);
      efault:
	err = -EFAULT;
	swerr();
	goto error;
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      ebusy:
	err = -EBUSY;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  N_INFORM_REQ
 *  -----------------------------------
 */
static inline int
n_inform_req(struct sc *sc, queue_t *q)
{
	int err;
	mblk_t *mp;
	N_inform_req_t *p;
	size_t msg_len = sizeof(*p);

	if (!canput(sc->oq))
		goto ebusy;
	if (!(mp = mi_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->PRIM_type = N_INFORM_REQ;
	printd(("%s: %p: N_INFORM_REQ ->\n", DRV_NAME, sc));
	put(sc->oq, mp);
	return (QR_ABSORBED);
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      ebusy:
	err = -EBUSY;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  N_COORD_REQ
 *  -----------------------------------
 */
static inline int
n_coord_req(struct sc *sc, queue_t *q, struct sccp_addr *add)
{
	int err;
	mblk_t *mp;
	N_coord_req_t *p;
	size_t add_len = add ? sizeof(*add) + add->alen : 0;
	size_t msg_len = sizeof(*p) + add_len;

	if (!canput(sc->oq))
		goto ebusy;
	if (!(mp = mi_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->PRIM_type = N_COORD_REQ;
	p->ADDR_length = add_len;
	p->ADDR_offset = add_len ? sizeof(*p) : 0;
	if (add_len) {
		bcopy(add, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
	}
	printd(("%s: %p: N_COORD_REQ ->\n", DRV_NAME, sc));
	put(sc->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      ebusy:
	err = -EBUSY;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  N_COORD_RES
 *  -----------------------------------
 */
static inline int
n_coord_res(struct sc *sc, queue_t *q, struct sccp_addr *add)
{
	int err;
	mblk_t *mp;
	N_coord_res_t *p;
	size_t add_len = add ? sizeof(*add) + add->alen : 0;
	size_t msg_len = sizeof(*p) + add_len;

	if (!canput(sc->oq))
		goto ebusy;
	if (!(mp = mi_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->PRIM_type = N_COORD_RES;
	p->ADDR_length = add_len;
	p->ADDR_offset = add_len ? sizeof(*p) : 0;
	if (add_len) {
		bcopy(add, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
	}
	printd(("%s: %p: N_COORD_RES ->\n", DRV_NAME, sc));
	put(sc->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      ebusy:
	err = -EBUSY;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  N_STATE_REQ
 *  -----------------------------------
 */
static inline int
n_state_req(struct sc *sc, queue_t *q, struct sccp_addr *add, t_uscalar_t status)
{
	int err;
	mblk_t *mp;
	N_state_req_t *p;
	size_t add_len = add ? sizeof(*add) + add->alen : 0;
	size_t msg_len = sizeof(*p) + add_len;

	if (!canput(sc->oq))
		goto ebusy;
	if (!(mp = mi_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->PRIM_type = N_STATE_REQ;
	p->ADDR_length = add_len;
	p->ADDR_offset = add_len ? sizeof(*p) : 0;
	p->STATUS = status;
	if (add_len) {
		bcopy(add, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
	}
	printd(("%s: %p: N_STATE_REQ ->\n", DRV_NAME, sc));
	put(sc->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      ebusy:
	err = -EBUSY;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  =========================================================================
 *
 *  TCAP SEND Message Functions
 *
 *  =========================================================================
 */

/*
 *  TCAP_MT_UNI
 *  -----------------------------------
 */
static int
tcap_send_uni(struct sc *sc, queue_t *q, struct tr_msg *m)
{
	fixme(("Write this function\n"));
	return (-EFAULT);
}

/*
 *  TCAP_MT_QWP
 *  -----------------------------------
 */
static int
tcap_send_qwp(struct sc *sc, queue_t *q, struct tr_msg *m)
{
	fixme(("Write this function\n"));
	return (-EFAULT);
}

/*
 *  TCAP_MT_QWOP
 *  -----------------------------------
 */
static int
tcap_send_qwop(struct sc *sc, queue_t *q, struct tr_msg *m)
{
	fixme(("Write this function\n"));
	return (-EFAULT);
}

/*
 *  TCAP_MT_CWP
 *  -----------------------------------
 */
static int
tcap_send_cwp(struct sc *sc, queue_t *q, struct tr_msg *m)
{
	fixme(("Write this function\n"));
	return (-EFAULT);
}

/*
 *  TCAP_MT_CWOP
 *  -----------------------------------
 */
static int
tcap_send_cwop(struct sc *sc, queue_t *q, struct tr_msg *m)
{
	fixme(("Write this function\n"));
	return (-EFAULT);
}

/*
 *  TCAP_MT_RESP
 *  -----------------------------------
 */
static int
tcap_send_resp(struct sc *sc, queue_t *q, struct tr_msg *m)
{
	fixme(("Write this function\n"));
	return (-EFAULT);
}

/*
 *  TCAP_MT_ABORT
 *  -----------------------------------
 */
static int
tcap_send_abort(struct sc *sc, queue_t *q, struct tr_msg *m)
{
	fixme(("Write this function\n"));
	return (-EFAULT);
}

static inline int
tcap_send_msg(struct sc *sc, queue_t *q, struct tr_msg *m)
{
	int ret;

	switch (m->type) {
	case TCAP_MT_UNI:
		ret = tcap_send_uni(sc, q, m);
		break;
	case TCAP_MT_QWP:
		ret = tcap_send_qwp(sc, q, m);
		break;
	case TCAP_MT_QWOP:
		ret = tcap_send_qwop(sc, q, m);
		break;
	case TCAP_MT_CWP:
		ret = tcap_send_cwp(sc, q, m);
		break;
	case TCAP_MT_CWOP:
		ret = tcap_send_cwop(sc, q, m);
		break;
	case TCAP_MT_RESP:
		ret = tcap_send_resp(sc, q, m);
		break;
	case TCAP_MT_ABORT:
		ret = tcap_send_abort(sc, q, m);
		break;
	default:
		ret = -EFAULT;
		break;
	}
	return (ret);
}

/*
 *  =========================================================================
 *
 *  TCAP RECV Message Functions
 *
 *  =========================================================================
 */
/*
 *  TCAP_MT_UNI
 *  -----------------------------------
 */
static int
tcap_recv_uni(struct tc *tc, queue_t *q, struct tcap_opts *opt, struct tr_msg *m, mblk_t *dp)
{
	int err;
	t_uscalar_t did = 0;
	t_uscalar_t flags = (m->parms & TCAP_PTF_CSEQ) ? 1 : 0;

	todo(("Fill out other options\n"));
	switch (tc->i_style) {
	case TCAP_STYLE_TRI:
		if ((err = tr_uni_ind(tc, q, &m->sc.orig, &m->sc.dest, opt, dp)) < 0)
			goto error;
		break;
	case TCAP_STYLE_TCI:
		if ((err = tc_uni_ind(tc, q, &m->sc.orig, &m->sc.dest, opt, did, flags, dp)) < 0)
			goto error;
		/* Provide a TC_UNI_IND and follow with any TC_XXX_IND component indications. */
		fixme(("Send components.\n"));
		break;
	case TCAP_STYLE_TPI:
		if ((err = t_unitdata_ind(tc, q, &m->sc.orig, opt, dp)) < 0)
			goto error;
		break;
	default:
		err = -EFAULT;
		swerr();
		goto error;
	}
	return (QR_TRIMMED);
      error:
	return (err);
}

/*
 *  TCAP_MT_QWP
 *  -----------------------------------
 */
static int
tcap_recv_qwp(struct tc *tc, queue_t *q, struct tcap_opts *opt, struct tr_msg *m, mblk_t *dp)
{
	int err;

	switch (tc->i_style) {
	case TCAP_STYLE_TRI:
		if ((err = tr_begin_ind(tc, q, &m->sc.orig, &m->sc.dest, opt, m->origid, dp)) < 0)
			goto error;
		break;
	case TCAP_STYLE_TCI:
		if ((err =
		     tc_begin_ind(tc, q, &m->sc.orig, &m->sc.dest, opt, m->origid,
				  (m->cmp_beg != m->cmp_end), dp)) < 0)
			goto error;
		/* Provide a TC_BEG_IND and follow with any TC_XXX_IND component indications. */
		fixme(("Send components\n"));
		break;
	case TCAP_STYLE_TPI:
		if ((err = t_conn_ind(tc, q, dp)) < 0)
			goto error;
		break;
	default:
		err = -EFAULT;
		swerr();
		goto error;
	}
	return (QR_TRIMMED);
      error:
	return (err);
}

/*
 *  TCAP_MT_QWOP
 *  -----------------------------------
 */
static int
tcap_recv_qwop(struct tc *tc, queue_t *q, struct tcap_opts *opt, struct tr_msg *m, mblk_t *dp)
{
	int err;

	switch (tc->i_style) {
	case TCAP_STYLE_TRI:
		if ((err = tr_begin_ind(tc, q, &m->sc.orig, &m->sc.dest, opt, m->origid, dp)) < 0)
			goto error;
		break;
	case TCAP_STYLE_TCI:
		if ((err =
		     tc_begin_ind(tc, q, &m->sc.orig, &m->sc.dest, opt, m->origid,
				  (m->cmp_beg != m->cmp_end), dp)) < 0)
			goto error;
		/* Provide a TC_BEG_IND and follow with any TC_XXX_IND component indications. */
		fixme(("Send components\n"));
		break;
	case TCAP_STYLE_TPI:
		if ((err = t_conn_ind(tc, q, dp)) < 0)
			goto error;
		break;
	default:
		err = -EFAULT;
		swerr();
		goto error;
	}
	return (QR_TRIMMED);
      error:
	return (err);
}

/*
 *  TCAP_MT_CWP
 *  -----------------------------------
 */
static int
tcap_recv_cwp(struct tc *tc, queue_t *q, struct tcap_opts *opt, struct tr_msg *m, mblk_t *dp)
{
	int err;

	switch (tc->i_style) {
	case TCAP_STYLE_TRI:
		if ((err = tr_begin_con(tc, q, &m->sc.orig, opt, m->destid, 0, dp)))
			goto error;
		break;
	case TCAP_STYLE_TCI:
		if ((err = tc_begin_con(tc, q, opt, m->destid, (m->cmp_beg != m->cmp_end), dp)))
			goto error;
		/* Provide a TC_BEG_CON or TC_CONT_IND and follow with any TC_XXX_IND component
		   indications. */
		fixme(("Send components.\n"));
		break;
	case TCAP_STYLE_TPI:
		if ((err = t_conn_con(tc, q, &m->sc.orig, m->sc.pcl, dp)))
			goto error;
		break;
	}
	return (QR_TRIMMED);
      error:
	return (err);
}

/*
 *  TCAP_MT_CWOP
 *  -----------------------------------
 */
static int
tcap_recv_cwop(struct tc *tc, queue_t *q, struct tcap_opts *opt, struct tr_msg *m, mblk_t *dp)
{
	int err;

	switch (tc->i_style) {
	case TCAP_STYLE_TRI:
		if ((err = tr_begin_con(tc, q, &m->sc.orig, opt, m->destid, 0, dp)))
			goto error;
		break;
	case TCAP_STYLE_TCI:
		if ((err = tc_begin_con(tc, q, opt, m->destid, (m->cmp_beg != m->cmp_end), dp)))
			goto error;
		/* Provide a TC_BEG_CON or TC_CONT_IND and follow with any TC_XXX_IND component
		   indications. */
		fixme(("Send components.\n"));
		break;
	case TCAP_STYLE_TPI:
		if ((err = t_conn_con(tc, q, &m->sc.orig, m->sc.pcl, dp)))
			goto error;
		break;
	}
	return (QR_TRIMMED);
      error:
	return (err);
}

/*
 *  TCAP_MT_RESP
 *  -----------------------------------
 */
static int
tcap_recv_resp(struct tc *tc, queue_t *q, struct tcap_opts *opt, struct tr_msg *m, mblk_t *dp)
{
	int err;

	switch (tc->i_style) {
	case TCAP_STYLE_TRI:
		if ((err = tr_end_ind(tc, q, opt, m->destid, 0, dp)))
			goto error;
		break;
	case TCAP_STYLE_TCI:
		if ((err = tc_end_ind(tc, q, opt, m->destid, (m->cmp_beg != m->cmp_end), dp)))
			goto error;
		/* Provide a TC_END_IND and follow with any TC_XXX_IND component indications. */
		fixme(("Send components.\n"));
		break;
	case TCAP_STYLE_TPI:
		if ((err = t_ordrel_ind(tc, q)))
			goto error;
		break;
	}
	return (QR_TRIMMED);
      error:
	return (err);
}

/*
 *  TCAP_MT_ABORT
 *  -----------------------------------
 */
static int
tcap_recv_abort(struct tc *tc, queue_t *q, struct tcap_opts *opt, struct tr_msg *m, mblk_t *dp)
{
	int err;

	switch (tc->i_style) {
	case TCAP_STYLE_TRI:
		if ((err = tr_abort_ind(tc, q, m->cause, 0, opt, m->destid, 0)))
			goto error;
		break;
	case TCAP_STYLE_TCI:
		if ((err = tc_abort_ind(tc, q, opt, m->destid, m->cause, 0)))
			goto error;
		fixme(("Send components.\n"));
		/* Provide a TC_ABORT_IND. */
		break;
	case TCAP_STYLE_TPI:
		if ((err = t_discon_ind(tc, q, m->cause, 0, dp)))
			goto error;
		break;
	}
	return (QR_TRIMMED);
      error:
	return (err);
}

static int
tcap_recv_msg(queue_t *q, struct tcap_opts *opt, struct tr_msg *m, mblk_t *dp)
{
	struct tc *tc = TC_PRIV(q);
	int ret;

	switch (m->type) {
	case TCAP_MT_UNI:
		ret = tcap_recv_uni(tc, q, opt, m, dp);
		break;
	case TCAP_MT_QWP:
		ret = tcap_recv_qwp(tc, q, opt, m, dp);
		break;
	case TCAP_MT_QWOP:
		ret = tcap_recv_qwop(tc, q, opt, m, dp);
		break;
	case TCAP_MT_CWP:
		ret = tcap_recv_cwp(tc, q, opt, m, dp);
		break;
	case TCAP_MT_CWOP:
		ret = tcap_recv_cwop(tc, q, opt, m, dp);
		break;
	case TCAP_MT_RESP:
		ret = tcap_recv_resp(tc, q, opt, m, dp);
		break;
	case TCAP_MT_ABORT:
		ret = tcap_recv_abort(tc, q, opt, m, dp);
		break;
	default:
		ret = -EOPNOTSUPP;
		break;
	}
	return (ret);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Received Primitives
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  -------------------------------------------------------------------------
 *
 *  Primitives received from above (TCAP User).
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  TCI Primitives
 *  -----------------------------------------------------------
 */
/*
 *  TC_INFO_REQ
 *  -----------------------------------------------------------
 */
static int
tc_info_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	int err;
	struct TC_info_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badprim:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tc));
	goto error;
      error:
	return tc_error_ack(tc, q, p->PRIM_type, err, 0, 0);
}

/*
 *  TC_BIND_REQ
 *  -----------------------------------------------------------
 */
static int
tc_bind_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	int err;
	struct TC_bind_req *p = (typeof(p)) mp->b_rptr;
	struct sccp_addr *add = NULL;
	t_uscalar_t xact = 0, flags = 0;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (p->ADDR_length) {
		if (mp->b_wptr < mp->b_rptr + p->ADDR_offset + p->ADDR_length)
			goto badprim;
		if (p->ADDR_length < sizeof(*add))
			goto badaddr;
		add = (typeof(add)) (mp->b_rptr + p->ADDR_offset);
		if (p->ADDR_length != sizeof(*add) + add->alen)
			goto badaddr;
	}
	xact = p->XACT_number;
	flags = p->BIND_flags;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badaddr:
	err = TCBADADDR;
	ptrace(("%s: %p: ERROR: bad address format\n", DRV_NAME, tc));
	goto error;
      badprim:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tc));
	goto error;
      error:
	return tc_error_ack(tc, q, p->PRIM_type, err, 0, 0);
}

/*
 *  TC_UNBIND_REQ
 *  -----------------------------------------------------------
 */
static int
tc_unbind_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	int err;
	struct TC_unbind_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (tc->dg.list)
		goto outstate;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      outstate:
	err = TCOUTSTATE;
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, tc));
	goto error;
      badprim:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tc));
	goto error;
      error:
	return tc_error_ack(tc, q, p->PRIM_type, err, 0, 0);
}

/*
 *  TC_SUBS_BIND_REQ
 *  -----------------------------------------------------------
 */
static int
tc_subs_bind_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	int err;
	struct TC_subs_bind_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badprim:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tc));
	goto error;
      error:
	return tc_error_ack(tc, q, p->PRIM_type, err, 0, 0);
}

/*
 *  TC_SUBS_UNBIND_REQ
 *  -----------------------------------------------------------
 */
static int
tc_subs_unbind_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	int err;
	struct TC_subs_unbind_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badprim:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tc));
	goto error;
      error:
	return tc_error_ack(tc, q, p->PRIM_type, err, 0, 0);
}

/*
 *  TC_OPTMGMT_REQ
 *  -----------------------------------------------------------
 */
static int
tc_optmgmt_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	int err;
	struct TC_optmgmt_req *p = (typeof(p)) mp->b_rptr;
	struct tcap_opts opts = { 0UL, };
	t_uscalar_t flags = 0;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (p->OPT_length) {
		if (mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length)
			goto badprim;
		if (tcap_parse_opts(tc, &opts, mp->b_rptr + p->OPT_offset, p->OPT_length))
			goto badopt;
	}
	flags = p->MGMT_flags;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badopt:
	err = TCBADOPT;
	ptrace(("%s: %p: ERROR: bad options format\n", DRV_NAME, tc));
	goto error;
      badprim:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tc));
	goto error;
      error:
	return tc_error_ack(tc, q, p->PRIM_type, err, 0, 0);
}

/*
 *  TC_UNI_REQ
 *  -----------------------------------------------------------
 */
static int
tc_uni_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	int err;
	struct TC_uni_req *p = (typeof(p)) mp->b_rptr;
	struct sccp_addr *src = NULL;
	struct sccp_addr *dst = NULL;
	struct tcap_opts opts = { 0UL, };
	struct dg *dg;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (p->SRC_length) {
		if (mp->b_wptr < mp->b_rptr + p->SRC_offset + p->SRC_length)
			goto badprim;
		if (p->SRC_length < sizeof(*src))
			goto badaddr;
		src = (typeof(src)) (mp->b_rptr + p->SRC_offset);
		if (p->SRC_length != sizeof(*src) + src->alen)
			goto badaddr;
	}
	if (p->DEST_length) {
		if (mp->b_wptr < mp->b_rptr + p->DEST_offset + p->DEST_length)
			goto badprim;
		if (p->DEST_length < sizeof(*dst))
			goto badaddr;
		dst = (typeof(dst)) (mp->b_rptr + p->DEST_offset);
		if (p->DEST_length != sizeof(*dst) + dst->alen)
			goto badaddr;
	}
	if (p->OPT_length) {
		if (mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length)
			goto badprim;
		if (tcap_parse_opts(tc, &opts, mp->b_rptr + p->OPT_offset, p->OPT_length))
			goto badopt;
	}
	if (!(dg = dg_lookup(tc, p->DIALOG_id)))
		goto badaddr;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badopt:
	err = TCBADOPT;
	ptrace(("%s: %p: ERROR: bad options format\n", DRV_NAME, tc));
	goto error;
      badaddr:
	err = TCBADADDR;
	ptrace(("%s: %p: ERROR: bad address format\n", DRV_NAME, tc));
	goto error;
      badprim:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tc));
	goto error;
      error:
	return tc_error_ack(tc, q, p->PRIM_type, err, p->DIALOG_id, 0);
}

/*
 *  TC_BEGIN_REQ
 *  -----------------------------------------------------------
 */
static int
tc_begin_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	int err;
	struct TC_begin_req *p = (typeof(p)) mp->b_rptr;
	struct sccp_addr *src = NULL;
	struct sccp_addr *dst = NULL;
	struct tcap_opts opts = { 0UL, };
	t_uscalar_t flags;
	struct dg *dg;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (p->SRC_length) {
		if (mp->b_wptr < mp->b_rptr + p->SRC_offset + p->SRC_length)
			goto badprim;
		if (p->SRC_length < sizeof(*src))
			goto badaddr;
		src = (typeof(src)) (mp->b_rptr + p->SRC_offset);
		if (p->SRC_length != sizeof(*src) + src->alen)
			goto badaddr;
	}
	if (p->DEST_length) {
		if (mp->b_wptr < mp->b_rptr + p->DEST_offset + p->DEST_length)
			goto badprim;
		if (p->DEST_length < sizeof(*dst))
			goto badaddr;
		dst = (typeof(dst)) (mp->b_rptr + p->DEST_offset);
		if (p->DEST_length != sizeof(*dst) + dst->alen)
			goto badaddr;
	}
	if (p->OPT_length) {
		if (mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length)
			goto badprim;
		if (tcap_parse_opts(tc, &opts, mp->b_rptr + p->OPT_offset, p->OPT_length))
			goto badopt;
	}
	if (!(dg = dg_lookup(tc, p->DIALOG_id)))
		goto badaddr;
	flags = p->COMP_flags;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badopt:
	err = TCBADOPT;
	ptrace(("%s: %p: ERROR: bad options format\n", DRV_NAME, tc));
	goto error;
      badaddr:
	err = TCBADADDR;
	ptrace(("%s: %p: ERROR: bad address format\n", DRV_NAME, tc));
	goto error;
      badprim:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tc));
	goto error;
      error:
	return tc_error_ack(tc, q, p->PRIM_type, err, p->DIALOG_id, 0);
}

/*
 *  TC_BEGIN_RES
 *  -----------------------------------------------------------
 */
static int
tc_begin_res(struct tc *tc, queue_t *q, mblk_t *mp)
{
	int err;
	struct TC_begin_res *p = (typeof(p)) mp->b_rptr;
	struct sccp_addr *src = NULL;
	struct tcap_opts opts = { 0UL, };
	t_uscalar_t flags;
	struct dg *dg;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (p->SRC_length) {
		if (mp->b_wptr < mp->b_rptr + p->SRC_offset + p->SRC_length)
			goto badprim;
		if (p->SRC_length < sizeof(*src))
			goto badaddr;
		src = (typeof(src)) (mp->b_rptr + p->SRC_offset);
		if (p->SRC_length != sizeof(*src) + src->alen)
			goto badaddr;
	}
	if (p->OPT_length) {
		if (mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length)
			goto badprim;
		if (tcap_parse_opts(tc, &opts, mp->b_rptr + p->OPT_offset, p->OPT_length))
			goto badopt;
	}
	if (!(dg = dg_lookup(tc, p->DIALOG_id)))
		goto badaddr;
	flags = p->COMP_flags;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badopt:
	err = TCBADOPT;
	ptrace(("%s: %p: ERROR: bad options format\n", DRV_NAME, tc));
	goto error;
      badaddr:
	err = TCBADADDR;
	ptrace(("%s: %p: ERROR: bad address format\n", DRV_NAME, tc));
	goto error;
      badprim:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tc));
	goto error;
      error:
	return tc_error_ack(tc, q, p->PRIM_type, err, p->DIALOG_id, 0);
}

/*
 *  TC_CONT_REQ
 *  -----------------------------------------------------------
 */
static int
tc_cont_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	int err;
	struct TC_cont_req *p = (typeof(p)) mp->b_rptr;
	struct tcap_opts opts = { 0UL, };
	t_uscalar_t flags;
	struct dg *dg;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (p->OPT_length) {
		if (mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length)
			goto badprim;
		if (tcap_parse_opts(tc, &opts, mp->b_rptr + p->OPT_offset, p->OPT_length))
			goto badopt;
	}
	if (!(dg = dg_lookup(tc, p->DIALOG_id)))
		goto badaddr;
	flags = p->COMP_flags;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badopt:
	err = TCBADOPT;
	ptrace(("%s: %p: ERROR: bad options format\n", DRV_NAME, tc));
	goto error;
      badaddr:
	err = TCBADADDR;
	ptrace(("%s: %p: ERROR: bad address format\n", DRV_NAME, tc));
	goto error;
      badprim:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tc));
	goto error;
      error:
	return tc_error_ack(tc, q, p->PRIM_type, err, p->DIALOG_id, 0);
}

/*
 *  TC_END_REQ
 *  -----------------------------------------------------------
 */
static int
tc_end_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	int err;
	struct TC_end_req *p = (typeof(p)) mp->b_rptr;
	struct tcap_opts opts = { 0UL, };
	t_uscalar_t scenario;
	struct dg *dg;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (p->OPT_length) {
		if (mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length)
			goto badprim;
		if (tcap_parse_opts(tc, &opts, mp->b_rptr + p->OPT_offset, p->OPT_length))
			goto badopt;
	}
	if (!(dg = dg_lookup(tc, p->DIALOG_id)))
		goto badaddr;
	scenario = p->TERM_scenario;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badopt:
	err = TCBADOPT;
	ptrace(("%s: %p: ERROR: bad options format\n", DRV_NAME, tc));
	goto error;
      badaddr:
	err = TCBADADDR;
	ptrace(("%s: %p: ERROR: bad address format\n", DRV_NAME, tc));
	goto error;
      badprim:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tc));
	goto error;
      error:
	return tc_error_ack(tc, q, p->PRIM_type, err, p->DIALOG_id, 0);
}

/*
 *  TC_ABORT_REQ
 *  -----------------------------------------------------------
 */
static int
tc_abort_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	int err;
	struct TC_abort_req *p = (typeof(p)) mp->b_rptr;
	struct tcap_opts opts = { 0UL, };
	t_uscalar_t reason;
	struct dg *dg;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (p->OPT_length) {
		if (mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length)
			goto badprim;
		if (tcap_parse_opts(tc, &opts, mp->b_rptr + p->OPT_offset, p->OPT_length))
			goto badopt;
	}
	if (!(dg = dg_lookup(tc, p->DIALOG_id)))
		goto badaddr;
	reason = p->ABORT_reason;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badopt:
	err = TCBADOPT;
	ptrace(("%s: %p: ERROR: bad options format\n", DRV_NAME, tc));
	goto error;
      badaddr:
	err = TCBADADDR;
	ptrace(("%s: %p: ERROR: bad address format\n", DRV_NAME, tc));
	goto error;
      badprim:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tc));
	goto error;
      error:
	return tc_error_ack(tc, q, p->PRIM_type, err, p->DIALOG_id, 0);
}

/*
 *  TC_INVOKE_REQ
 *  -----------------------------------------------------------
 */
static int
tc_invoke_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	int err;
	struct TC_invoke_req *p = (typeof(p)) mp->b_rptr;
	t_uscalar_t pcls, oper, more, to;
	struct dg *dg;
	struct iv *iv, *li;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (!(dg = dg_lookup(tc, p->DIALOG_id)))
		goto badaddr;
	if (p->LINKED_id && !(li = iv_lookup(dg, p->LINKED_id)))
		goto badaddr;
	if (!(iv = iv_lookup(dg, p->INVOKE_id)))
		goto badaddr;
	pcls = p->PROTOCOL_class;
	oper = p->OPERATION;
	more = p->MORE_flag;
	to = p->TIMEOUT;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badaddr:
	err = TCBADADDR;
	ptrace(("%s: %p: ERROR: bad address format\n", DRV_NAME, tc));
	goto error;
      badprim:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tc));
	goto error;
      error:
	return tc_error_ack(tc, q, p->PRIM_type, err, p->DIALOG_id, p->INVOKE_id);
}

/*
 *  TC_RESULT_REQ
 *  -----------------------------------------------------------
 */
static int
tc_result_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	int err;
	struct TC_result_req *p = (typeof(p)) mp->b_rptr;
	t_uscalar_t oper, more;
	struct dg *dg;
	struct iv *iv;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (!(dg = dg_lookup(tc, p->DIALOG_id)))
		goto badaddr;
	if (!(iv = iv_lookup(dg, p->INVOKE_id)))
		goto badaddr;
	oper = p->OPERATION;
	more = p->MORE_flag;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badaddr:
	err = TCBADADDR;
	ptrace(("%s: %p: ERROR: bad address format\n", DRV_NAME, tc));
	goto error;
      badprim:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tc));
	goto error;
      error:
	return tc_error_ack(tc, q, p->PRIM_type, err, p->DIALOG_id, p->INVOKE_id);
}

/*
 *  TC_ERROR_REQ
 *  -----------------------------------------------------------
 */
static int
tc_error_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	int err;
	struct TC_error_req *p = (typeof(p)) mp->b_rptr;
	t_uscalar_t ecode, more;
	struct dg *dg;
	struct iv *iv;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (!(dg = dg_lookup(tc, p->DIALOG_id)))
		goto badaddr;
	if (!(iv = iv_lookup(dg, p->INVOKE_id)))
		goto badaddr;
	ecode = p->ERROR_code;
	more = p->MORE_flag;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badaddr:
	err = TCBADADDR;
	ptrace(("%s: %p: ERROR: bad address format\n", DRV_NAME, tc));
	goto error;
      badprim:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tc));
	goto error;
      error:
	return tc_error_ack(tc, q, p->PRIM_type, err, p->DIALOG_id, p->INVOKE_id);
}

/*
 *  TC_CANCEL_REQ
 *  -----------------------------------------------------------
 */
static int
tc_cancel_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	int err;
	struct TC_cancel_req *p = (typeof(p)) mp->b_rptr;
	struct dg *dg;
	struct iv *iv;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (!(dg = dg_lookup(tc, p->DIALOG_id)))
		goto badaddr;
	if (!(iv = iv_lookup(dg, p->INVOKE_id)))
		goto badaddr;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badaddr:
	err = TCBADADDR;
	ptrace(("%s: %p: ERROR: bad address format\n", DRV_NAME, tc));
	goto error;
      badprim:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tc));
	goto error;
      error:
	return tc_error_ack(tc, q, p->PRIM_type, err, p->DIALOG_id, p->INVOKE_id);
}

/*
 *  TC_REJECT_REQ
 *  -----------------------------------------------------------
 */
static int
tc_reject_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	int err;
	struct TC_reject_req *p = (typeof(p)) mp->b_rptr;
	t_uscalar_t pcode;
	struct dg *dg;
	struct iv *iv;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (!(dg = dg_lookup(tc, p->DIALOG_id)))
		goto badaddr;
	if (!(iv = iv_lookup(dg, p->INVOKE_id)))
		goto badaddr;
	pcode = p->PROBLEM_code;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badaddr:
	err = TCBADADDR;
	ptrace(("%s: %p: ERROR: bad address format\n", DRV_NAME, tc));
	goto error;
      badprim:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tc));
	goto error;
      error:
	return tc_error_ack(tc, q, p->PRIM_type, err, p->DIALOG_id, p->INVOKE_id);
}

static int
tc_other_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	t_uscalar_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	goto notsupp;
      notsupp:
	err = TCNOTSUPPORT;
	mi_strlog(q, STRLOGRX, SL_TRACE, "primitive not supported %u", (uint) *p);
	goto error;
      badprim:
	err = -EINVAL;
	mi_strlog(q, STRLOGRX, SL_TRACE, "bad primitive format");
	goto error;
      error:
	return tc_error_ack(tc, q, *p, err, 0, 0);
}

/*
 *  TRI Primitives
 *  -----------------------------------------------------------
 */
/*
 *  TR_INFO_REQ
 *  -----------------------------------------------------------
 */
static int
tr_info_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	int err;
	struct TR_info_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badprim:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tc));
	goto error;
      error:
	return tr_error_ack(tc, q, p->PRIM_type, err, 0);
}

/*
 *  TR_BIND_REQ
 *  -----------------------------------------------------------
 */
static int
tr_bind_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	int err;
	struct TR_bind_req *p = (typeof(p)) mp->b_rptr;
	struct sccp_addr *add = NULL;
	t_uscalar_t xact, flags;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (p->ADDR_length) {
		if (mp->b_wptr < mp->b_rptr + p->ADDR_offset + p->ADDR_length)
			goto badprim;
		if (p->ADDR_length < sizeof(*add))
			goto badaddr;
		add = (typeof(add)) (mp->b_rptr + p->ADDR_offset);
		if (p->ADDR_length != sizeof(*add) + add->alen)
			goto badaddr;
	}
	xact = p->XACT_number;
	flags = p->BIND_flags;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badaddr:
	err = TRBADADDR;
	ptrace(("%s: %p: ERROR: bad address format\n", DRV_NAME, tc));
	goto error;
      badprim:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tc));
	goto error;
      error:
	return tr_error_ack(tc, q, p->PRIM_type, err, 0);
}

/*
 *  TR_UNBIND_REQ
 *  -----------------------------------------------------------
 */
static int
tr_unbind_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	int err;
	struct TR_unbind_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badprim:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tc));
	goto error;
      error:
	return tr_error_ack(tc, q, p->PRIM_type, err, 0);
}

/*
 *  TR_OPTMGMT_REQ
 *  -----------------------------------------------------------
 */
static int
tr_optmgmt_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	int err;
	struct TR_optmgmt_req *p = (typeof(p)) mp->b_rptr;
	struct tcap_opts opts = { 0, };
	t_uscalar_t flags;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (p->OPT_length) {
		if (mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length)
			goto badprim;
		if (tcap_parse_opts(tc, &opts, mp->b_rptr + p->OPT_offset, p->OPT_length))
			goto badopt;
	}
	flags = p->MGMT_flags;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badopt:
	err = TRBADOPT;
	ptrace(("%s: %p: ERROR: bad options format\n", DRV_NAME, tc));
	goto error;
      badprim:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tc));
	goto error;
      error:
	return tr_error_ack(tc, q, p->PRIM_type, err, 0);
}

/*
 *  TR_UNI_REQ
 *  -----------------------------------------------------------
 */
static int
tr_uni_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	int err;
	struct TR_uni_req *p = (typeof(p)) mp->b_rptr;
	struct sccp_addr *dst = NULL;
	struct sccp_addr *org = NULL;
	struct tcap_opts opts = { 0, };

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (p->DEST_length) {
		if (mp->b_wptr < mp->b_rptr + p->DEST_offset + p->DEST_length)
			goto badprim;
		if (p->DEST_length < sizeof(*dst))
			goto badaddr;
		dst = (typeof(dst)) (mp->b_rptr + p->DEST_offset);
		if (p->DEST_length != sizeof(*dst) + dst->alen)
			goto badaddr;
	}
	if (p->ORIG_length) {
		if (mp->b_wptr < mp->b_rptr + p->ORIG_offset + p->ORIG_length)
			goto badprim;
		if (p->ORIG_length < sizeof(*org))
			goto badaddr;
		org = (typeof(org)) (mp->b_rptr + p->ORIG_offset);
		if (p->ORIG_length != sizeof(*org) + org->alen)
			goto badaddr;
	}
	if (p->OPT_length) {
		if (mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length)
			goto badprim;
		if (tcap_parse_opts(tc, &opts, mp->b_rptr + p->OPT_offset, p->OPT_length))
			goto badopt;
	}
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badopt:
	err = TRBADOPT;
	ptrace(("%s: %p: ERROR: bad options format\n", DRV_NAME, tc));
	goto error;
      badaddr:
	err = TRBADADDR;
	ptrace(("%s: %p: ERROR: bad address format\n", DRV_NAME, tc));
	goto error;
      badprim:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tc));
	goto error;
      error:
	return tr_error_ack(tc, q, p->PRIM_type, err, 0);
}

/*
 *  TR_BEGIN_REQ
 *  -----------------------------------------------------------
 */
static int
tr_begin_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	int err;
	struct TR_begin_req *p = (typeof(p)) mp->b_rptr;
	struct sccp_addr *dst = NULL;
	struct sccp_addr *org = NULL;
	struct tcap_opts opts = { 0, };
	struct tr *tr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (p->DEST_length) {
		if (mp->b_wptr < mp->b_rptr + p->DEST_offset + p->DEST_length)
			goto badprim;
		if (p->DEST_length < sizeof(*dst))
			goto badaddr;
		dst = (typeof(dst)) (mp->b_rptr + p->DEST_offset);
		if (p->DEST_length != sizeof(*dst) + dst->alen)
			goto badaddr;
	}
	if (p->ORIG_length) {
		if (mp->b_wptr < mp->b_rptr + p->ORIG_offset + p->ORIG_length)
			goto badprim;
		if (p->ORIG_length < sizeof(*org))
			goto badaddr;
		org = (typeof(org)) (mp->b_rptr + p->ORIG_offset);
		if (p->ORIG_length != sizeof(*org) + org->alen)
			goto badaddr;
	}
	if (p->OPT_length) {
		if (mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length)
			goto badprim;
		if (tcap_parse_opts(tc, &opts, mp->b_rptr + p->OPT_offset, p->OPT_length))
			goto badopt;
	}
	if (!p->CORR_id || tr_lookup_cid(tc, p->CORR_id))
		goto badaddr;
	if (!(tr = tcap_alloc_tr(tc, p->CORR_id, 0)))
		goto enomem;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badopt:
	err = TRBADOPT;
	ptrace(("%s: %p: ERROR: bad options format\n", DRV_NAME, tc));
	goto error;
      badaddr:
	err = TRBADADDR;
	ptrace(("%s: %p: ERROR: bad address format\n", DRV_NAME, tc));
	goto error;
      badprim:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tc));
	goto error;
      enomem:
	err = -ENOMEM;
	goto error;
      error:
	return tr_error_ack(tc, q, p->PRIM_type, err, p->CORR_id);
}

/*
 *  TR_BEGIN_RES
 *  -----------------------------------------------------------
 */
static int
tr_begin_res(struct tc *tc, queue_t *q, mblk_t *mp)
{
	int err;
	struct TR_begin_res *p = (typeof(p)) mp->b_rptr;
	struct sccp_addr *org = NULL;
	struct tcap_opts opts = { 0, };
	struct tr *tr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (p->ORIG_length) {
		if (mp->b_wptr < mp->b_rptr + p->ORIG_offset + p->ORIG_length)
			goto badprim;
		if (p->ORIG_length < sizeof(*org))
			goto badaddr;
		org = (typeof(org)) (mp->b_rptr + p->ORIG_offset);
		if (p->ORIG_length != sizeof(*org) + org->alen)
			goto badaddr;
	}
	if (p->OPT_length) {
		if (mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length)
			goto badprim;
		if (tcap_parse_opts(tc, &opts, mp->b_rptr + p->OPT_offset, p->OPT_length))
			goto badopt;
	}
	if (!(tr = tr_lookup_tid(tc, p->TRANS_id)))
		goto badtid;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badtid:
	err = TRBADADDR;
	ptrace(("%s: %p: ERROR: bad transaction id\n", DRV_NAME, tc));
	goto error;
      badopt:
	err = TRBADOPT;
	ptrace(("%s: %p: ERROR: bad options format\n", DRV_NAME, tc));
	goto error;
      badaddr:
	err = TRBADADDR;
	ptrace(("%s: %p: ERROR: bad address format\n", DRV_NAME, tc));
	goto error;
      badprim:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tc));
	goto error;
      error:
	return tr_error_ack(tc, q, p->PRIM_type, err, p->TRANS_id);
}

/*
 *  TR_CONT_REQ
 *  -----------------------------------------------------------
 */
static int
tr_cont_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	int err;
	struct TR_cont_req *p = (typeof(p)) mp->b_rptr;
	struct tcap_opts opts = { 0, };
	struct tr *tr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (p->OPT_length) {
		if (mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length)
			goto badprim;
		if (tcap_parse_opts(tc, &opts, mp->b_rptr + p->OPT_offset, p->OPT_length))
			goto badopt;
	}
	if (!(tr = tr_lookup_tid(tc, p->TRANS_id)))
		goto badtid;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badtid:
	err = TRBADADDR;
	ptrace(("%s: %p: ERROR: bad transaction id\n", DRV_NAME, tc));
	goto error;
      badopt:
	err = TRBADOPT;
	ptrace(("%s: %p: ERROR: bad options format\n", DRV_NAME, tc));
	goto error;
      badprim:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tc));
	goto error;
      error:
	return tr_error_ack(tc, q, p->PRIM_type, err, p->TRANS_id);
}

/*
 *  TR_END_REQ
 *  -----------------------------------------------------------
 */
static int
tr_end_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	int err;
	struct TR_end_req *p = (typeof(p)) mp->b_rptr;
	struct tcap_opts opts = { 0, };
	t_uscalar_t term;
	struct tr *tr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (p->OPT_length) {
		if (mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length)
			goto badprim;
		if (tcap_parse_opts(tc, &opts, mp->b_rptr + p->OPT_offset, p->OPT_length))
			goto badopt;
	}
	if (!(tr = tr_lookup_tid(tc, p->TRANS_id)))
		goto badtid;
	term = p->TERM_scenario;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badtid:
	err = TRBADADDR;
	ptrace(("%s: %p: ERROR: bad transaction id\n", DRV_NAME, tc));
	goto error;
      badopt:
	err = TRBADOPT;
	ptrace(("%s: %p: ERROR: bad options format\n", DRV_NAME, tc));
	goto error;
      badprim:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tc));
	goto error;
      error:
	return tr_error_ack(tc, q, p->PRIM_type, err, p->TRANS_id);
}

/*
 *  TR_ABORT_REQ
 *  -----------------------------------------------------------
 */
static int
tr_abort_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	int err;
	struct TR_abort_req *p = (typeof(p)) mp->b_rptr;
	struct tcap_opts opts = { 0, };
	t_uscalar_t cause;
	struct tr *tr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (p->OPT_length) {
		if (mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length)
			goto badprim;
		if (tcap_parse_opts(tc, &opts, mp->b_rptr + p->OPT_offset, p->OPT_length))
			goto badopt;
	}
	if (!(tr = tr_lookup_tid(tc, p->TRANS_id)))
		goto badtid;
	cause = p->ABORT_cause;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badtid:
	err = TRBADADDR;
	ptrace(("%s: %p: ERROR: bad transaction id\n", DRV_NAME, tc));
	goto error;
      badopt:
	err = TRBADOPT;
	ptrace(("%s: %p: ERROR: bad options format\n", DRV_NAME, tc));
	goto error;
      badprim:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tc));
	goto error;
      error:
	return tr_error_ack(tc, q, p->PRIM_type, err, p->TRANS_id);
}

static int
tr_other_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	t_uscalar_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	goto notsupp;
      notsupp:
	err = TRNOTSUPPORT;
	mi_strlog(q, STRLOGRX, SL_TRACE, "primitive not supported %u", (uint) *p);
	goto error;
      badprim:
	err = -EINVAL;
	mi_strlog(q, STRLOGRX, SL_TRACE, "bad primitive format");
	goto error;
      error:
	return tr_error_ack(tc, q, *p, err, 0);
}

/*
 *  TPI Primitives
 *  -----------------------------------------------------------
 */
/*
 *  T_CONN_REQ
 *  -----------------------------------------------------------
 */
static int
t_conn_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	int err;
	struct T_conn_req *p = (typeof(p)) mp->b_rptr;
	struct sccp_addr *dst = NULL;
	struct tcap_opts opts = { 0UL, };

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (!p->DEST_length)
		goto noaddr;
	if (mp->b_wptr < mp->b_rptr + p->DEST_offset + p->DEST_length)
		goto badprim;
	if (p->DEST_length < sizeof(*dst))
		goto badprim;
	dst = (typeof(dst)) (mp->b_rptr + p->DEST_offset);
	if (p->DEST_length != sizeof(*dst) + dst->alen)
		goto badaddr;
	if (p->OPT_length) {
		if (mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length)
			goto badprim;
		if (!tcap_parse_opts(tc, &opts, mp->b_rptr + p->OPT_offset, p->OPT_length))
			goto badopt;
	}
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badopt:
	err = TBADOPT;
	ptrace(("%s: %p: ERROR: bad options format\n", DRV_NAME, tc));
	goto error;
      badaddr:
	err = TBADADDR;
	ptrace(("%s: %p: ERROR: bad address format\n", DRV_NAME, tc));
	goto error;
      noaddr:
	err = TNOADDR;
	ptrace(("%s: %p: ERROR: could not assign address\n", DRV_NAME, tc));
	goto error;
      badprim:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tc));
	goto error;
      error:
	return t_error_ack(tc, q, p->PRIM_type, err);
}

/*
 *  T_CONN_RES
 *  -----------------------------------------------------------
 */
static int
t_conn_res(struct tc *tc, queue_t *q, mblk_t *mp)
{
	int err;
	struct T_conn_res *p = (typeof(p)) mp->b_rptr;
	struct tcap_opts opts = { 0UL, };
	t_uscalar_t tok, seq;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (p->OPT_length) {
		if (mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length)
			goto badprim;
		if (!tcap_parse_opts(tc, &opts, mp->b_rptr + p->OPT_offset, p->OPT_length))
			goto badopt;
	}
	tok = p->ACCEPTOR_id;
	seq = p->SEQ_number;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badopt:
	err = TBADOPT;
	ptrace(("%s: %p: ERROR: bad options format\n", DRV_NAME, tc));
	goto error;
      badprim:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tc));
	goto error;
      error:
	return t_error_ack(tc, q, p->PRIM_type, err);
}

/*
 *  T_DISCON_REQ
 *  -----------------------------------------------------------
 */
static int
t_discon_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	int err;
	struct T_discon_req *p = (typeof(p)) mp->b_rptr;
	t_uscalar_t seq;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	seq = p->SEQ_number;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badprim:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tc));
	goto error;
      error:
	return t_error_ack(tc, q, p->PRIM_type, err);
}

/*
 *  T_DATA_REQ
 *  -----------------------------------------------------------
 */
static int
t_data_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	int err;
	struct T_data_req *p = (typeof(p)) mp->b_rptr;
	t_uscalar_t more;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	more = p->MORE_flag;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badprim:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tc));
	goto error;
      error:
	return t_error_ack(tc, q, p->PRIM_type, err);
}

/*
 *  T_EXDATA_REQ
 *  -----------------------------------------------------------
 */
static int
t_exdata_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	int err;
	struct T_exdata_req *p = (typeof(p)) mp->b_rptr;
	t_uscalar_t more;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	more = p->MORE_flag;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badprim:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tc));
	goto error;
      error:
	return t_error_ack(tc, q, p->PRIM_type, err);
}

/*
 *  T_INFO_REQ
 *  -----------------------------------------------------------
 */
static int
t_info_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	int err;
	struct T_info_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badprim:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tc));
	goto error;
      error:
	return t_error_ack(tc, q, p->PRIM_type, err);
}

/*
 *  T_BIND_REQ
 *  -----------------------------------------------------------
 */
static int
t_bind_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	int err;
	struct T_bind_req *p = (typeof(p)) mp->b_rptr;
	struct sccp_addr *add = NULL;
	t_uscalar_t cons;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (!p->ADDR_length)
		goto noaddr;
	if (mp->b_wptr < mp->b_rptr + p->ADDR_offset + p->ADDR_length)
		goto badprim;
	if (p->ADDR_length < sizeof(*add))
		goto badaddr;
	add = (typeof(add)) (mp->b_rptr + p->ADDR_offset);
	if (p->ADDR_length != sizeof(*add) + add->alen)
		goto badaddr;
	cons = p->CONIND_number;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badaddr:
	err = TBADADDR;
	ptrace(("%s: %p: ERROR: bad address format\n", DRV_NAME, tc));
	goto error;
      noaddr:
	err = TNOADDR;
	ptrace(("%s: %p: ERROR: could not assign address\n", DRV_NAME, tc));
	goto error;
      badprim:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tc));
	goto error;
      error:
	return t_error_ack(tc, q, p->PRIM_type, err);
}

/*
 *  T_UNBIND_REQ
 *  -----------------------------------------------------------
 */
static int
t_unbind_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	int err;
	struct T_unbind_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badprim:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tc));
	goto error;
      error:
	return t_error_ack(tc, q, p->PRIM_type, err);
}

/*
 *  T_UNITDATA_REQ
 *  -----------------------------------------------------------
 */
static int
t_unitdata_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	int err;
	struct T_unitdata_req *p = (typeof(p)) mp->b_rptr;
	struct sccp_addr *dst;
	struct tcap_opts opts = { 0UL, };

#if 0
	if (tc->ocls != 0)
		goto notsupport;
#endif
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (!p->DEST_length)
		goto noaddr;
	if (mp->b_wptr < mp->b_rptr + p->DEST_offset + p->DEST_length)
		goto badprim;
	if (p->DEST_length < sizeof(*dst))
		goto badaddr;
	dst = (typeof(dst)) (mp->b_rptr + p->DEST_offset);
	if (p->DEST_length != sizeof(*dst) + dst->alen)
		goto badaddr;
	if (p->OPT_length) {
		if (mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length)
			goto badprim;
		if (!tcap_parse_opts(tc, &opts, mp->b_rptr + p->OPT_offset, p->OPT_length))
			goto badopt;
	}
	/* 
	 *  Launch a UNIDIRECTIONAL to the destination address with the
	 *  options as specified.
	 */
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badopt:
	err = TBADOPT;
	ptrace(("%s: %p: ERROR: bad options format\n", DRV_NAME, tc));
	goto error;
      badaddr:
	err = TBADADDR;
	ptrace(("%s: %p: ERROR: bad address format\n", DRV_NAME, tc));
	goto error;
      noaddr:
	err = TNOADDR;
	ptrace(("%s: %p: ERROR: could not assign address\n", DRV_NAME, tc));
	goto error;
      badprim:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tc));
	goto error;
#if 0
      notsupport:
	err = TNOTSUPPORT;
	ptrace(("%s: %p: ERROR: primitive not supported for operations class\n", DRV_NAME, tc));
	goto error;
#endif
      error:
	return t_error_ack(tc, q, p->PRIM_type, err);
}

/*
 *  T_OPTMGMT_REQ
 *  -----------------------------------------------------------
 */
static int
t_optmgmt_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	int err;
	struct T_optmgmt_req *p = (typeof(p)) mp->b_rptr;
	struct tcap_opts opts = { 0UL, };
	t_uscalar_t flags;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (p->OPT_length) {
		if (mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length)
			goto badprim;
		if (!tcap_parse_opts(tc, &opts, mp->b_rptr + p->OPT_offset, p->OPT_length))
			goto badopt;
	}
	switch ((flags = p->MGMT_flags)) {
	case T_CURRENT:
	case T_NEGOTIATE:
	case T_DEFAULT:
	case T_CHECK:
		break;
	default:
		goto badflag;
	}
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badflag:
	err = TBADFLAG;
	ptrace(("%s: %p: ERROR: bad flags\n", DRV_NAME, tc));
	goto error;
      badopt:
	err = TBADOPT;
	ptrace(("%s: %p: ERROR: bad options format\n", DRV_NAME, tc));
	goto error;
      badprim:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tc));
	goto error;
      error:
	return t_error_ack(tc, q, p->PRIM_type, err);
}

/*
 *  T_ORDREL_REQ
 *  -----------------------------------------------------------
 */
static int
t_ordrel_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	int err;
	struct T_ordrel_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badprim:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tc));
	goto error;
      error:
	return t_error_ack(tc, q, p->PRIM_type, err);
}

#ifdef T_OPTDATA_REQ
/*
 *  T_OPTDATA_REQ
 *  -----------------------------------------------------------
 */
static int
t_optdata_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	int err;
	struct T_optdata_req *p = (typeof(p)) mp->b_rptr;
	struct tcap_opts opts = { 0UL, };
	t_uscalar_t flags;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (p->OPT_length) {
		if (mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length)
			goto badprim;
		if (!tcap_parse_opts(tc, &opts, mp->b_rptr + p->OPT_offset, p->OPT_length))
			goto badopt;
	}
	flags = p->DATA_flag;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badopt:
	err = TBADOPT;
	ptrace(("%s: %p: ERROR: bad options format\n", DRV_NAME, tc));
	goto error;
      badprim:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tc));
	goto error;
      error:
	return t_error_ack(tc, q, p->PRIM_type, err);
}
#endif

#ifdef T_ADDR_REQ
/*
 *  T_ADDR_REQ
 *  -----------------------------------------------------------
 */
static int
t_addr_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	int err;
	struct T_addr_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badprim:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tc));
	goto error;
      error:
	return t_error_ack(tc, q, p->PRIM_type, err);
}
#endif

#ifdef T_CAPABILITY_REQ
/*
 *  T_CAPABILITY_REQ
 *  -----------------------------------------------------------
 */
static int
t_capability_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	int err;
	struct T_capability_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badprim:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tc));
	goto error;
      error:
	return t_error_ack(tc, q, p->PRIM_type, err);
}
#endif

static int
t_other_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	t_uscalar_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	goto notsupp;
      notsupp:
	mi_strlog(q, STRLOGRX, SL_TRACE, "primitive not supported %u", (uint) *p);
	err = TNOTSUPPORT;
	goto error;
      badprim:
	mi_strlog(q, STRLOGRX, SL_TRACE, "bad primitive format");
	err = -EINVAL;
	goto error;
      error:
	return t_error_ack(tc, q, *p, err);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Primitives received from below (SCCP Provider).
 *
 *  -------------------------------------------------------------------------
 */

/*
 *  NPI (SCCP) Primitives
 *  -----------------------------------------------------------
 */
/*
 *  N_CONN_IND
 *  -----------------------------------------------------------
 */
static int
n_conn_ind(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_conn_ind_t *p = (typeof(p)) mp->b_rptr;
	struct sccp_addr *dst;
	N_qos_sel_conn_sccp_t qos = { N_QOS_SEL_CONN_SCCP, 0, };
	t_uscalar_t flags;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (!p->DEST_length)
		goto noaddr;
	if (mp->b_wptr < mp->b_rptr + p->DEST_offset + p->DEST_length)
		goto badprim;
	if (p->DEST_length < sizeof(*dst))
		goto badaddr;
	dst = (typeof(dst)) (mp->b_rptr + p->DEST_offset);
	if (p->DEST_length != sizeof(*dst) + dst->alen)
		goto badaddr;
	flags = p->CONN_flags;
	if (p->QOS_length) {
		if (mp->b_wptr < mp->b_rptr + p->QOS_offset + p->QOS_length)
			goto badprim;
		if (p->QOS_length < sizeof(qos))
			goto badqostype;
		bcopy(mp->b_rptr + p->QOS_offset, &qos, sizeof(qos));
		if (qos.n_qos_type != N_QOS_SEL_CONN_SCCP)
			goto badqostype;
		if (qos.protocol_class != 2 && qos.protocol_class != 3)
			goto badqosparam;
	}
	goto notsupport;
      notsupport:
	pswerr(("%s: %p: SWERR: unsupported primitive from below\n", DRV_NAME, sc));
	goto error;
      badqosparam:
	pswerr(("%s: %p: SWERR: bad QOS parameter value\n", DRV_NAME, sc));
	goto error;
      badqostype:
	pswerr(("%s: %p: SWERR: bad QOS structure type\n", DRV_NAME, sc));
	goto error;
      badaddr:
	pswerr(("%s: %p: SWERR: bad address\n", DRV_NAME, sc));
	goto error;
      noaddr:
	pswerr(("%s: %p: SWERR: could not assign address\n", DRV_NAME, sc));
	goto error;
      badprim:
	pswerr(("%s: %p: SWERR: invalid primitive format\n", DRV_NAME, sc));
	goto error;
      error:
	freemsg(mp);
	return (0);
}

/*
 *  N_CONN_CON
 *  -----------------------------------------------------------
 */
static int
n_conn_con(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_conn_con_t *p = (typeof(p)) mp->b_rptr;
	struct sccp_addr *res = &sc->rem;
	N_qos_sel_conn_sccp_t qos = { N_QOS_SEL_CONN_SCCP, 0, };
	t_uscalar_t flags;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (p->RES_length) {
		if (mp->b_wptr < mp->b_rptr + p->RES_offset + p->RES_length)
			goto badprim;
		if (p->RES_length < sizeof(*res))
			goto badaddr;
		res = (typeof(res)) (mp->b_rptr + p->RES_offset);
		if (p->RES_length < sizeof(*res) + res->alen)
			goto badaddr;
	}
	flags = p->CONN_flags;
	if (p->QOS_length) {
		if (mp->b_wptr < mp->b_rptr + p->QOS_offset + p->QOS_length)
			goto badprim;
		if (p->QOS_length < sizeof(qos))
			goto badqostype;
		bcopy(mp->b_rptr + p->QOS_offset, &qos, sizeof(qos));
		if (qos.n_qos_type != N_QOS_SEL_CONN_SCCP)
			goto badqostype;
		if (qos.protocol_class != 2 && qos.protocol_class != 3)
			goto badqosparam;
	}
	goto notsupport;
      notsupport:
	pswerr(("%s: %p: SWERR: unsupported primitive from below\n", DRV_NAME, sc));
	goto error;
      badqosparam:
	pswerr(("%s: %p: SWERR: bad QOS parameter value\n", DRV_NAME, sc));
	goto error;
      badqostype:
	pswerr(("%s: %p: SWERR: bad QOS structure type\n", DRV_NAME, sc));
	goto error;
      badaddr:
	pswerr(("%s: %p: SWERR: bad address\n", DRV_NAME, sc));
	goto error;
      badprim:
	pswerr(("%s: %p: SWERR: invalid primitive format\n", DRV_NAME, sc));
	goto error;
      error:
	freemsg(mp);
	return (0);
}

/*
 *  N_DISCON_IND
 *  -----------------------------------------------------------
 */
static int
n_discon_ind(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_discon_ind_t *p = (typeof(p)) mp->b_rptr;
	struct sccp_addr *res = &sc->rem;
	t_uscalar_t orig, reason, seq;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (p->RES_length) {
		if (mp->b_wptr < mp->b_rptr + p->RES_offset + p->RES_length)
			goto badprim;
		if (p->RES_length < sizeof(*res))
			goto badaddr;
		res = (typeof(res)) (mp->b_rptr + p->RES_offset);
		if (p->RES_length < sizeof(*res) + res->alen)
			goto badaddr;
	}
	orig = p->DISCON_orig;
	reason = p->DISCON_reason;
	seq = p->SEQ_number;
	goto notsupport;
      notsupport:
	pswerr(("%s: %p: SWERR: unsupported primitive from below\n", DRV_NAME, sc));
	goto error;
      badaddr:
	pswerr(("%s: %p: SWERR: bad address\n", DRV_NAME, sc));
	goto error;
      badprim:
	pswerr(("%s: %p: SWERR: invalid primitive format\n", DRV_NAME, sc));
	goto error;
      error:
	freemsg(mp);
	return (0);
}

/*
 *  N_DATA_IND
 *  -----------------------------------------------------------
 */
static int
n_data_ind(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_data_ind_t *p = (typeof(p)) mp->b_rptr;
	t_uscalar_t flags;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	flags = p->DATA_xfer_flags;
	goto notsupport;
      notsupport:
	pswerr(("%s: %p: SWERR: unsupported primitive from below\n", DRV_NAME, sc));
	goto error;
      badprim:
	pswerr(("%s: %p: SWERR: invalid primitive format\n", DRV_NAME, sc));
	goto error;
      error:
	freemsg(mp);
	return (0);
}

/*
 *  N_EXDATA_IND
 *  -----------------------------------------------------------
 */
static int
n_exdata_ind(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_exdata_ind_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	goto notsupport;
      notsupport:
	pswerr(("%s: %p: SWERR: unsupported primitive from below\n", DRV_NAME, sc));
	goto error;
      badprim:
	pswerr(("%s: %p: SWERR: invalid primitive format\n", DRV_NAME, sc));
	goto error;
      error:
	freemsg(mp);
	return (0);
}

/*
 *  N_INFO_ACK
 *  -----------------------------------------------------------
 */
static int
n_info_ack(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_info_ack_t *p = (typeof(p)) mp->b_rptr;
	struct sccp_addr *add = NULL;
	N_qos_sel_info_sccp_t qos;
	N_qos_range_info_sccp_t qor;
	uchar *pro = NULL;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (p->ADDR_length) {
		if (mp->b_wptr < mp->b_rptr + p->ADDR_offset + p->ADDR_length)
			goto badprim;
		if (p->ADDR_length < sizeof(*add))
			goto badaddr;
		add = (typeof(add)) (mp->b_rptr + p->ADDR_offset);
		if (p->ADDR_length < sizeof(*add) + add->alen)
			goto badaddr;
	}
	if (p->QOS_length) {
		if (mp->b_wptr < mp->b_rptr + p->QOS_offset + p->QOS_length)
			goto badprim;
		if (p->QOS_length < sizeof(qos))
			goto badqostype;
		bcopy(mp->b_rptr + p->QOS_offset, &qos, sizeof(qos));
		if (qos.n_qos_type != N_QOS_SEL_INFO_SCCP)
			goto badqostype;
		if (qos.protocol_class < 0 || qos.protocol_class > 3)
			goto badqosparam;
	}
	if (p->QOS_range_length) {
		if (mp->b_wptr < mp->b_rptr + p->QOS_range_offset + p->QOS_range_length)
			goto badprim;
		if (p->QOS_range_length < sizeof(qor))
			goto badqostype;
		bcopy(mp->b_rptr + p->QOS_range_offset, &qor, sizeof(qor));
		if (qor.n_qos_type != N_QOS_RANGE_INFO_SCCP)
			goto badqostype;
	}
	if (p->PROTOID_length) {
		if (mp->b_wptr < mp->b_rptr + p->PROTOID_offset + p->PROTOID_length)
			goto badprim;
		pro = (typeof(pro)) (mp->b_rptr + p->PROTOID_offset);
	}
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badqosparam:
	pswerr(("%s: %p: SWERR: bad QOS parameter value\n", DRV_NAME, sc));
	goto error;
      badqostype:
	pswerr(("%s: %p: SWERR: bad QOS structure type\n", DRV_NAME, sc));
	goto error;
      badaddr:
	pswerr(("%s: %p: SWERR: bad address\n", DRV_NAME, sc));
	goto error;
      badprim:
	pswerr(("%s: %p: SWERR: invalid primitive format\n", DRV_NAME, sc));
	goto error;
      error:
	freemsg(mp);
	return (0);
}

/*
 *  N_BIND_ACK
 *  -----------------------------------------------------------
 */
static int
n_bind_ack(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_bind_ack_t *p = (typeof(p)) mp->b_rptr;
	struct sccp_addr *add;
	uchar *pro = NULL;
	t_uscalar_t conind, tok;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (!p->ADDR_length)
		goto noaddr;
	if (mp->b_wptr < mp->b_rptr + p->ADDR_offset + p->ADDR_length)
		goto badprim;
	if (p->ADDR_length < sizeof(*add))
		goto badaddr;
	add = (typeof(add)) (mp->b_rptr + p->ADDR_offset);
	if (p->ADDR_length < sizeof(*add) + add->alen)
		goto badaddr;
	conind = p->CONIND_number;
	tok = p->TOKEN_value;
	if (p->PROTOID_length) {
		if (mp->b_wptr < mp->b_rptr + p->PROTOID_offset + p->PROTOID_length)
			goto badprim;
		pro = (typeof(pro)) (mp->b_rptr + p->PROTOID_offset);
	}
	goto notsupport;
      notsupport:
	pswerr(("%s: %p: SWERR: unsupported primitive from below\n", DRV_NAME, sc));
	goto error;
      badaddr:
	pswerr(("%s: %p: SWERR: bad address\n", DRV_NAME, sc));
	goto error;
      noaddr:
	pswerr(("%s: %p: SWERR: could not assign address\n", DRV_NAME, sc));
	goto error;
      badprim:
	pswerr(("%s: %p: SWERR: invalid primitive format\n", DRV_NAME, sc));
	goto error;
      error:
	freemsg(mp);
	return (0);
}

/*
 *  N_ERROR_ACK
 *  -----------------------------------------------------------
 */
static int
n_error_ack(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_error_ack_t *p = (typeof(p)) mp->b_rptr;
	t_uscalar_t prim, etype;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	prim = p->ERROR_prim;
	etype = (p->NPI_error == NSYSERR) ? -p->UNIX_error : p->NPI_error;
	goto notsupport;
      notsupport:
	pswerr(("%s: %p: SWERR: unsupported primitive from below\n", DRV_NAME, sc));
	goto error;
      badprim:
	pswerr(("%s: %p: SWERR: invalid primitive format\n", DRV_NAME, sc));
	goto error;
      error:
	freemsg(mp);
	return (0);
}

/*
 *  N_OK_ACK
 *  -----------------------------------------------------------
 */
static int
n_ok_ack(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_ok_ack_t *p = (typeof(p)) mp->b_rptr;
	t_uscalar_t prim;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	prim = p->CORRECT_prim;
	goto notsupport;
      notsupport:
	pswerr(("%s: %p: SWERR: unsupported primitive from below\n", DRV_NAME, sc));
	goto error;
      badprim:
	pswerr(("%s: %p: SWERR: invalid primitive format\n", DRV_NAME, sc));
	goto error;
      error:
	freemsg(mp);
	return (0);
}

/*
 *  N_UNITDATA_IND
 *  -----------------------------------------------------------
 */
static int
n_unitdata_ind(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_unitdata_ind_t *p = (typeof(p)) mp->b_rptr;
	struct tr_msg msg;
	struct tcap_opts opts;
	struct sccp_addr *src;
	struct sccp_addr *dst = &sc->add;
	mblk_t *dp;
	size_t dlen;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (!p->SRC_length)
		goto noaddr;
	if (mp->b_wptr < mp->b_rptr + p->SRC_offset + p->SRC_length)
		goto badprim;
	if (p->SRC_length < sizeof(*src))
		goto badaddr;
	src = (typeof(src)) (mp->b_rptr + p->SRC_offset);
	if (p->SRC_length != sizeof(*src) + src->alen)
		goto badaddr;
	bcopy(src, &msg.sc.orig, sizeof(*src) + src->alen);
	if (p->DEST_length) {
		if (mp->b_wptr < mp->b_rptr + p->DEST_offset + p->DEST_length)
			goto badprim;
		if (p->DEST_length < sizeof(*dst))
			goto badaddr;
		dst = (typeof(dst)) (mp->b_rptr + p->DEST_offset);
		if (p->DEST_length != sizeof(*dst) + dst->alen)
			goto badaddr;
		if (dst->ni != sc->add.ni || dst->pc != sc->add.pc || dst->ssn != sc->add.ssn)
			goto badaddr;
	}
	bcopy(dst, &msg.sc.dest, sizeof(*dst) + dst->alen);
#if 0
	if (p->QOS_length) {
		N_qos_sel_data_sccp_t qos = { N_QOS_SEL_DATA_SCCP, };

		if (mp->b_wptr < mp->b_rptr + p->DEST_offset + p->DEST_length)
			goto badprim;
		if (p->QOS_length < sizeof(qos))
			goto badqostype;
		bcopy(mp->b_rptr + QOS_offset, &qos, sizeof(qos));
		if (qos.n_qos_type != N_QOS_SEL_DATA_SCCP)
			goto badqostype;
		msg.sc.pcl = qos.protocol_class;
		msg.sc.ret = qos.option_flags;
		msg.sc.imp = qos.importance;
		msg.sc.seq = qos.sequence_control;
		msg.sc.sls = qos.sequence_control;
		msg.sc.mp = qos.message_priority;
	} else {
#endif
		msg.sc.pcl = sc->pcl;
		msg.sc.ret = sc->ret;
		msg.sc.imp = sc->imp;
		msg.sc.seq = sc->seq;
		msg.sc.sls = sc->sls;
		msg.sc.mp = sc->mp;
#if 0
	}
#endif
	opts.pcl = &msg.sc.pcl;
	opts.ret = &msg.sc.ret;
	opts.imp = &msg.sc.imp;
	opts.seq = &msg.sc.seq;
	opts.sls = &msg.sc.sls;
	opts.mp = &msg.sc.mp;
	opts.flags = (TF_OPT_PCL | TF_OPT_RET | TF_OPT_IMP | TF_OPT_SEQ | TF_OPT_SLS | TF_OPT_MP);
	if (!(dp = mp->b_cont) || !(dlen = msgdsize(dp)) || dlen > sc->prot.NSDU_size)
		goto baddata;
	if ((err = ss7_pullupmsg(q, dp, -1)) < 0)
		goto error;
	if ((err = tcap_dec_msg(dp->b_rptr, dp->b_wptr, &msg)) < 0)
		goto error;
	if ((err = tcap_recv_msg(q, &opts, &msg, dp)) < 0)
		goto error;
	return (err);
      baddata:
	pswerr(("%s: %p: SWERR: bad data\n", DRV_NAME, sc));
	goto error;
      badaddr:
	pswerr(("%s: %p: SWERR: bad address\n", DRV_NAME, sc));
	goto error;
      noaddr:
	pswerr(("%s: %p: SWERR: could not assign address\n", DRV_NAME, sc));
	goto error;
      badprim:
	pswerr(("%s: %p: SWERR: invalid primitive format\n", DRV_NAME, sc));
	goto error;
      error:
	freemsg(mp);
	return (0);
}

/*
 *  N_UDERROR_IND
 *  -----------------------------------------------------------
 */
static int
n_uderror_ind(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_uderror_ind_t *p = (typeof(p)) mp->b_rptr;
	struct sccp_addr *dst;
	t_uscalar_t etype;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (!p->DEST_length)
		goto noaddr;
	if (mp->b_wptr < mp->b_rptr + p->DEST_offset + p->DEST_length)
		goto badprim;
	if (p->DEST_length < sizeof(*dst))
		goto badaddr;
	dst = (typeof(dst)) (mp->b_rptr + p->DEST_offset);
	if (p->DEST_length < sizeof(*dst) + dst->alen)
		goto badaddr;
	etype = p->ERROR_type;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badaddr:
	pswerr(("%s: %p: SWERR: bad address\n", DRV_NAME, sc));
	goto error;
      noaddr:
	pswerr(("%s: %p: SWERR: could not assign address\n", DRV_NAME, sc));
	goto error;
      badprim:
	pswerr(("%s: %p: SWERR: invalid primitive format\n", DRV_NAME, sc));
	goto error;
      error:
	freemsg(mp);
	return (0);
}

/*
 *  N_DATACK_IND
 *  -----------------------------------------------------------
 */
static int
n_datack_ind(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_datack_ind_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	goto notsupport;
      notsupport:
	pswerr(("%s: %p: SWERR: unsupported primitive from below\n", DRV_NAME, sc));
	goto error;
      badprim:
	pswerr(("%s: %p: SWERR: invalid primitive format\n", DRV_NAME, sc));
	goto error;
      error:
	freemsg(mp);
	return (0);
}

/*
 *  N_RESET_IND
 *  -----------------------------------------------------------
 */
static int
n_reset_ind(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_reset_ind_t *p = (typeof(p)) mp->b_rptr;
	t_uscalar_t orig, reason;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	orig = p->RESET_orig;
	reason = p->RESET_reason;
	goto notsupport;
      notsupport:
	pswerr(("%s: %p: SWERR: unsupported primitive from below\n", DRV_NAME, sc));
	goto error;
      badprim:
	pswerr(("%s: %p: SWERR: invalid primitive format\n", DRV_NAME, sc));
	goto error;
      error:
	freemsg(mp);
	return (0);
}

/*
 *  N_RESET_CON
 *  -----------------------------------------------------------
 */
static int
n_reset_con(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_reset_con_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	goto notsupport;
      notsupport:
	pswerr(("%s: %p: SWERR: unsupported primitive from below\n", DRV_NAME, sc));
	goto error;
      badprim:
	pswerr(("%s: %p: SWERR: invalid primitive format\n", DRV_NAME, sc));
	goto error;
      error:
	freemsg(mp);
	return (0);
}

/*
 *  N_COORD_IND
 *  -----------------------------------------------------------
 */
static int
n_coord_ind(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_coord_ind_t *p = (typeof(p)) mp->b_rptr;
	struct sccp_addr *add;
	t_uscalar_t smi;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (!p->ADDR_length)
		goto noaddr;
	if (mp->b_wptr < mp->b_rptr + p->ADDR_offset + p->ADDR_length)
		goto badprim;
	if (p->ADDR_length < sizeof(*add))
		goto badaddr;
	add = (typeof(add)) (mp->b_rptr + p->ADDR_offset);
	if (p->ADDR_length != sizeof(*add) + add->alen)
		goto badaddr;
	smi = p->SMI;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badaddr:
	pswerr(("%s: %p: SWERR: bad address\n", DRV_NAME, sc));
	goto error;
      noaddr:
	pswerr(("%s: %p: SWERR: could not assign address\n", DRV_NAME, sc));
	goto error;
      badprim:
	pswerr(("%s: %p: SWERR: invalid primitive format\n", DRV_NAME, sc));
	goto error;
      error:
	freemsg(mp);
	return (0);
}

/*
 *  N_COORD_CON
 *  -----------------------------------------------------------
 */
static int
n_coord_con(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_coord_con_t *p = (typeof(p)) mp->b_rptr;
	struct sccp_addr *add;
	t_uscalar_t smi;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (!p->ADDR_length)
		goto noaddr;
	if (mp->b_wptr < mp->b_rptr + p->ADDR_offset + p->ADDR_length)
		goto badprim;
	if (p->ADDR_length < sizeof(*add))
		goto badaddr;
	add = (typeof(add)) (mp->b_rptr + p->ADDR_offset);
	if (p->ADDR_length != sizeof(*add) + add->alen)
		goto badaddr;
	smi = p->SMI;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badaddr:
	pswerr(("%s: %p: SWERR: bad address\n", DRV_NAME, sc));
	goto error;
      noaddr:
	pswerr(("%s: %p: SWERR: could not assign address\n", DRV_NAME, sc));
	goto error;
      badprim:
	pswerr(("%s: %p: SWERR: invalid primitive format\n", DRV_NAME, sc));
	goto error;
      error:
	freemsg(mp);
	return (0);
}

/*
 *  N_STATE_IND
 *  -----------------------------------------------------------
 */
static int
n_state_ind(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_state_ind_t *p = (typeof(p)) mp->b_rptr;
	struct sccp_addr *add;
	t_uscalar_t status, smi;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (!p->ADDR_length)
		goto noaddr;
	if (mp->b_wptr < mp->b_rptr + p->ADDR_offset + p->ADDR_length)
		goto badprim;
	if (p->ADDR_length < sizeof(*add))
		goto badaddr;
	add = (typeof(add)) (mp->b_rptr + p->ADDR_offset);
	if (p->ADDR_length != sizeof(*add) + add->alen)
		goto badaddr;
	status = p->STATUS;
	smi = p->SMI;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badaddr:
	pswerr(("%s: %p: SWERR: bad address\n", DRV_NAME, sc));
	goto error;
      noaddr:
	pswerr(("%s: %p: SWERR: could not assign address\n", DRV_NAME, sc));
	goto error;
      badprim:
	pswerr(("%s: %p: SWERR: invalid primitive format\n", DRV_NAME, sc));
	goto error;
      error:
	freemsg(mp);
	return (0);
}

/**
 * n_pcstate_ind: - process an N_PCSTATE_IND primitive
 * @sc: SCCP private structure
 * @q: active queue (lower multiplex read queue)
 * @mp: the N_PCSTATE_IND primitive
 */
static int
n_pcstate_ind(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_pcstate_ind_t *p = (typeof(p)) mp->b_rptr;
	struct mtp_addr *add;
	t_uscalar_t status;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (!p->ADDR_length)
		goto noaddr;
	if (mp->b_wptr < mp->b_rptr + p->ADDR_offset + p->ADDR_length)
		goto badprim;
	if (p->ADDR_length < sizeof(*add))
		goto badaddr;
	add = (typeof(add)) (mp->b_rptr + p->ADDR_offset);
	if (p->ADDR_length != sizeof(*add))
		goto badaddr;
	status = p->STATUS;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badaddr:
	pswerr(("%s: %p: SWERR: bad address\n", DRV_NAME, sc));
	goto error;
      noaddr:
	pswerr(("%s: %p: SWERR: could not assign address\n", DRV_NAME, sc));
	goto error;
      badprim:
	pswerr(("%s: %p: SWERR: invalid primitive format\n", DRV_NAME, sc));
	goto error;
      error:
	freemsg(mp);
	return (0);
}

static int
n_other_ind(struct sc *sc, queue_t *q, mblk_t *mp)
{
	mi_strlog(q, 0, SL_ERROR, "unrecognized primitive on lower read queue %u",
			*(uint *)mp->b_rptr);
	freemsg(mp);
	return (0);
}

/*
 *  =========================================================================
 *
 *  STREAMS INPUT-OUTPUT CONTROL PROCESSING
 *
 *  =========================================================================
 */

static rwlock_t tcap_mux_lock = RW_LOCK_UNLOCKED;
static wait_queue_head_t tcap_waitq;

static void tcap_attach(struct tc *tc, struct te *te)
{
	if ((tc->te.next = te->tc.list))
		tc->te.next->te.prev = &tc->te.next;
	tc->te.prev = &te->tc.list;
	tc->te.te = te;
	te->tc.list = tc;
}

static void tcap_detach(struct tc *tc)
{
	if ((*tc->te.prev = tc->te.next))
		tc->te.next->te.prev = tc->te.prev;
	tc->te.next = NULL;
	tc->te.prev = &tc->te.next;
	tc->te.te = NULL;
	/* FIXME: detach from dialog */
}

static void sccp_link(struct sc *sc, struct te *te)
{
	if ((sc->te.next = te->sc.list))
		sc->te.next->te.prev = &sc->te.next;
	sc->te.prev = &te->sc.list;
	sc->te.te = te;
	te->sc.list = sc;
}

static void sccp_unlink(struct sc *sc)
{
	struct sp *sp;

	if ((*sc->te.prev = sc->te.next))
		sc->te.next->te.prev = sc->te.prev;
	sc->te.next = NULL;
	sc->te.prev = &sc->te.next;
	sc->te.te = NULL;
	if ((sp = sc->sp.sp)) {
		sp->sc.sc = NULL;
		sc->sp.sp = NULL;
	}
}

/**
 * tcap_link: - process an I_LINK M_IOCTL message
 * @lm: TCAP private structure with associated TE unlocked
 * @q: active queue (upper write queue)
 * @mp: the I_LINK M_IOCTL message
 *
 * The driver supports I_LINK operations; however, any SCCP stream that is linked with an I_LINK
 * operation can only be managed by the control Stream linking the lower Stream and cannot be shared
 * across other upper Streams unless configured against an SPID.
 *
 * Note that if this is not the first SCCP linked and there are running User, this SCCP will not be
 * available to them until it is configured and brought to the NS_IDLE state.  If this is the first
 * SCCP Stream, there cannot be running users.
 */
static noinline fastcall __unlikely int
tcap_i_link(struct tc *lm, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct linkblk *l = (typeof(l)) mp->b_cont->b_rptr;
	N_info_req_t *p;
	unsigned long flags;
	struct te *te, *te_new = NULL;
	struct sc *sc = NULL;
	mblk_t *rp = NULL;
	int err;

	if (!(rp = mi_allocb(q, sizeof(*p), BPRI_MED)))
		goto enobufs;
	if (!(sc = sccp_alloc_link(l->l_qtop, l->l_index, ioc->ioc_cr, 0)))
		goto enomem;
	/* might need an TE structure */
	if (!(te_new = tcap_alloc_te(lm, 0)))
		goto enomem;
	write_lock_irqsave(&tcap_mux_lock, flags);
	if (!(te = lm->te.te)) {
		te = XCHG(&te_new, NULL);
		te->sq.users = 1;
		te->lm.lm = lm;
	} else {
		if (!te_trylock(te, q)) {
			write_unlock_irqrestore(&tcap_mux_lock, flags);
			goto edeadlk;
		}
		/* Note that there can only be one layer management Stream per TE.  For temporary
		   links, that must be the same layer management Stream used to create the TE. */
		if (te->lm.lm != lm) {
			te_unlock(te);
			write_unlock_irqrestore(&tcap_mux_lock, flags);
			goto eperm;
		}
	}
	/* link the SCCP stream to the associated TCAP Entity */
	sccp_link(sc, te);

	mi_attach(l->l_qtop, (caddr_t) sc);
	te_unlock(te);
	write_unlock_irqrestore(&tcap_mux_lock, flags);
	if (te_new)
		tcap_free_te(te_new);
	mi_copy_done(q, mp, 0);

	/* Issue immediate information request. */
	DB_TYPE(rp) = M_PCPROTO;
	p = (typeof(p)) rp->b_rptr;
	p->PRIM_type = N_INFO_REQ;
	rp->b_wptr += sizeof(*p);
	putnext(sc->oq, rp);

	return (0);

      eperm:
	err = EPERM;
	goto error;
      edeadlk:
	err = -EDEADLK;
	goto error;
      enomem:
	err = ENOMEM;
	goto error;
      enobufs:
	err = -ENOBUFS;
	goto error;
      error:
	if (rp)
		freeb(rp);
	if (sc)
		sccp_free_unlink(sc);
	if (te_new)
		tcap_free_te(te_new);
	if (err < 0)
		return (err);
	mi_copy_done(q, mp, err);
	return (0);
}

/**
 * tcap_i_unlink: - process an I_PLINK M_IOCTL message
 * @lm: TCAP private structure with associated TE unlocked
 * @q: active queue (upper write queue)
 * @mp: the I_PLINK M_IOCTL message
 *
 * STREAMS ensures that I_UNLINK will only arrive for temporary links that were formed using this
 * control Stream.  Not all I_UNLINK operations can be refused.  I_UNLINK operations that result
 * from tearing down of the multiplex due to close of the control Stream cannot be refused.  In
 * general, there is no way to distinguish between an explicit unlink on the control Stream and an
 * implicit unlink resulting from the closing of the Stream.  For Linux Fast-STREAMS and possibly
 * Solaris there is: the ioc_flag member will have IOC_NONE for forced unlinks not originating from
 * a user and IOC_NATIVE or IOC_ILP32 for I_UNLINK operations originating from the user.
 *
 * Note that the lower multiplex driver put and service procedures must be prepared to be invoked
 * even after the M_IOCACK for the I_UNLINK or I_PUNLINK ioctl has been returned.  This is because
 * the setq(9) back to the Stream head is not performed until after the acknowledgement has been
 * received.  The easiest way to accomplish this is to set q->q_ptr to NULL and have the put and
 * service procedures for the lower multiplex check the private structure point for NULL (under
 * suitable lock).
 */
static noinline fastcall __unlikely int
tcap_i_unlink(struct tc *lm, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_wptr;
	struct linkblk *l = (typeof(l)) mp->b_cont->b_rptr;
	psw_t flags;
	struct te *te;
	struct sc *sc;
	int err;

	write_lock_irqsave(&tcap_mux_lock, flags);
	if (!(sc = SC_PRIV(l->l_qtop)))
		goto einval;
	if ((ioc->ioc_flag & IOC_MASK) == IOC_NONE) {
		/* if issued by user, check credentials */
		if (drv_priv(ioc->ioc_cr) != 0 && ioc->ioc_cr->cr_uid != sc->cred.cr_uid)
			goto eperm;
	}
	if ((te = sc->te.te)) {
		if (!te_trylock(te, q))
			goto edeadlk;

		/* XXX: we might need to abort any trasnactions associated with the unlinking SC,
		   or it is possible that we just mark the SP as disconnected and wait for timers
		   to fire or the TC User to attempt to communicate with the peer and cancel or
		   abort at that time. */

		sccp_unlink(sc);
	} else {
		/* It is actually an error for a temporary link not to be associated with a TCAP
		   Entity, but we should not issue errors with interrupts suppressed. */
	}

	mi_detach(l->l_qtop, (caddr_t) sc);
	/* Note: temporary links do not get included in the tcap_links device list. */
	te_unlock(te);
	write_unlock_irqrestore(&tcap_mux_lock, flags);

	sccp_free_unlink(sc);

	/* should probably flush queues in case a Stream head is reattached. */
	flushq(RD(l->l_qtop), FLUSHDATA);
	mi_copy_done(q, mp, 0);
	return (0);
      edeadlk:
	err = -EDEADLK;
	goto error;
      einval:
	err = EINVAL;
	goto error;
      eperm:
	err = EPERM;
	goto error;
      error:
	write_unlock_irqrestore(&tcap_mux_lock, flags);
	if (err < 0)
		return (err);
	mi_copy_done(q, mp, err);
	return (0);
}

/**
 * tcap_i_plink: - process an I_PLINK M_IOCTL message
 * @lm: TCAP private structure with associated TE unlocked
 * @q: active queue (upper write queue)
 * @mp: the I_PLINK M_IOCTL message
 *
 * With I_PUNLINK operations, the lower SCCP Stream can be shared across other upper Streams.  The
 * credentials of the linker are used to control access to the lower Stream.  The lower Stream
 * cannot be configured or unlinked without the privileges associated with the linking process.  Any
 * upper Stream; however, can bind to a lower Stream that was permanently linked.
 *
 * Note that if the linking Stream is associated with an TE, the linked Stream will also be
 * associated with that TE.  Otherwise, the linked Stream will simply be added to the list of lower
 * permanent links and it will have to be configured by MUX id.
 */
static noinline fastcall __unlikely int
tcap_i_plink(struct tc *lm, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct linkblk *l = (typeof(l)) mp->b_cont->b_rptr;
	N_info_req_t *p;
	psw_t flags;
	struct te *te = NULL;
	struct sc *sc = NULL;
	mblk_t *rp = NULL;
	int err;

	if (!(rp = mi_allocb(q, sizeof(*p), BPRI_MED)))
		goto enobufs;
	/* Sneaky trick.  If a non-zero minor device number was opened and on which the linking was
	   performed, then this TEID is ipmlied by the minor device number that was opened. */
	if (!(sc = sccp_alloc_link(l->l_qtop, l->l_index, ioc->ioc_cr, lm->te.te)))
		goto enomem;

	write_lock_irqsave(&tcap_mux_lock, flags);

	if (lm_ctrl != lm) {
		/* Only allow the master control Strem to do permanent links. */
		write_unlock_irqrestore(&tcap_mux_lock, flags);
		goto eperm;
	}

	if ((err = mi_open_link(&tcap_links, (caddr_t) sc, NULL, 0, MODOPEN, ioc->ioc_cr))) {
		write_unlock_irqrestore(&tcap_mux_lock, flags);
		goto error;
	}
	if ((te = lm->te.te)) {
		if (!te_trylock(te, q)) {
			mi_close_unlink(&tcap_links, (caddr_t) sc);
			write_unlock_irqrestore(&tcap_mux_lock, flags);
			goto edeadlk;
		}
		if (te->lm.lm != NULL) {
			/* Only one layer management Stream is permitted per TE.  If the TE has a
			   temporary layer manager, we cannot do permanent links against it. */
			te_unlock(te);
			mi_close_unlink(&tcap_links, (caddr_t) sc);
			write_unlock_irqrestore(&tcap_mux_lock, flags);
			goto eperm;
		}
		/* link the SCCP stream to the associated TCAP Entity */
		sccp_link(sc, te);
	} else {
		/* The SCCP stream is simply not associated with any TE at this point when the
		   master control stream is used (as it is not associated with any TE).  It
		   requires a later TCAP_OBJ_TYPE_SC configuration command to associate the SCCP
		   stream with SCCP SAP and TCAP Entity objects. */
	}

	mi_attach(l->l_qtop, (caddr_t) sc);
	if (te)
		te_unlock(te);
	write_unlock_irqrestore(&tcap_mux_lock, flags);
	mi_copy_done(q, mp, 0);

	DB_TYPE(rp) = M_PCPROTO;
	p = (typeof(p)) rp->b_rptr;
	p->PRIM_type = N_INFO_REQ;
	rp->b_wptr += sizeof(*p);
	putnext(sc->oq, rp);	/* immediate info request */

	return (0);
      enobufs:
	err = -ENOBUFS;
	goto error;
      enomem:
	err = ENOMEM;
	goto error;
      eperm:
	err = EPERM;
	goto error;
      edeadlk:
	err = -EDEADLK;
	goto error;
      error:
	if (rp)
		freeb(rp);
	if (sc)
		sccp_free_unlink(sc);
	if (err < 0)
		return (err);
	mi_copy_done(q, mp, err);
	return (0);
}

/**
 * tcap_i_punlink: - process an I_PUNLINK M_IOCTL message
 * @lm: TCAP private structure with associated TE unlocked
 * @q: active queue (upper write queue)
 * @mp: the I_PUNLINK M_IOCTL message
 *
 * STREAMS ensure that I_PUNLINK will only arrive for permanent links that were formed using this
 * driver.  All I_PUNLINK operations can be refused.  Only refuse to unlink when the unlinking
 * process does not have sufficient privilege.  The unlinking process must either be the super user
 * or the owner of the link.  All associated Users are addresses before the final unlinking.
 */
static noinline fastcall __unlikely int
tcap_i_punlink(struct tc *lm, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_wptr;
	struct linkblk *l = (typeof(l)) mp->b_cont->b_rptr;
	psw_t flags;
	struct te *te;
	struct sc *sc;
	int err;

	write_lock_irqsave(&tcap_mux_lock, flags);
	if (!(sc = SC_PRIV(l->l_qtop)))
		goto einval;
	/* Always issued by user, check credentials.  Only the master control Stream is allowed to
	   create permanent links, however, to avoid difficulties with hanging permanent links,
	   however, permit the owner of the link or the super user to undo permanent links. */
	if (lm != lm_ctrl && drv_priv(ioc->ioc_cr) != 0 && ioc->ioc_cr->cr_uid != sc->cred.cr_uid)
		goto eperm;
	if ((te = sc->te.te)) {
		if (!te_trylock(te, q))
			goto edeadlk;

		/* XXX: we might need to abort any trasnactions associated with the unlinking SC,
		   or it is possible that we just mark the SP as disconnected and wait for timers
		   to fire or the TC User to attempt to communicate with the peer and cancel or
		   abort at that time. */

		sccp_unlink(sc);
	} else {
		/* Note that the SCCP stream could be disassociated with a TCAP entity, so this is
		   not an error. */
	}

	mi_detach(l->l_qtop, (caddr_t) sc);
	mi_close_unlink(&tcap_links, (caddr_t) sc);
	te_unlock(te);
	write_unlock_irqrestore(&tcap_mux_lock, flags);

	sccp_free_unlink(sc);

	/* should probably flush queues in case a Stream head is reattached. */
	flushq(RD(l->l_qtop), FLUSHDATA);
	mi_copy_done(q, mp, 0);
	return (0);

      edeadlk:
	err = -EDEADLK;
	goto error;
      einval:
	err = EINVAL;
	goto error;
      eperm:
	err = EPERM;
	goto error;
      error:
	write_unlock_irqrestore(&tcap_mux_lock, flags);
	if (err < 0)
		return (err);
	mi_copy_done(q, mp, err);
	return (0);
}

/**
 * mgmt_ioctl: - process a STREAMS input-output control
 * @q: active queue (upper write queue)
 * @mp: the STREAMS M_IOCTL message
 */
static noinline fastcall __unlikely int
mgmt_ioctl(queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct tc *tc = TC_PRIV(q);
	int rtn;

	switch (_IOC_NR(ioc->ioc_cmd)) {
	case _IOC_NR(I_LINK):
		mi_strlog(q, STRLOGIO, SL_TRACE, "-> I_LINK");
		rtn = tcap_i_link(tc, q, mp);
		break;
	case _IOC_NR(I_UNLINK):
		mi_strlog(q, STRLOGIO, SL_TRACE, "-> I_UNLINK");
		rtn = tcap_i_unlink(tc, q, mp);
		break;
	case _IOC_NR(I_PLINK):
		mi_strlog(q, STRLOGIO, SL_TRACE, "-> I_PLINK");
		rtn = tcap_i_plink(tc, q, mp);
		break;
	case _IOC_NR(I_PUNLINK):
		mi_strlog(q, STRLOGIO, SL_TRACE, "-> I_PUNLINK");
		rtn = tcap_i_punlink(tc, q, mp);
		break;
	default:
		rtn = EINVAL;
		break;
	}
	if (rtn < 0)
		return (rtn);
	if (rtn > 0)
		mi_copy_done(q, mp, rtn);
	return (0);
}

/*
 *  =========================================================================
 *
 *  TCAP INPUT-OUTPUT CONTROL PROCESSING
 *
 *  =========================================================================
 */

/**
 * tcap_ioctl: - process M_IOCTL message for TCAP
 * @tc: TCAP private structure
 * @q: active queue (upper write queue)
 * @mp: the M_IOCTL message
 *
 * This is step 1 of the TCAP input-output control operation.  Step 1 always consists of a copyin
 * operation to determin the object type and id.
 */
static noinline __unlikely int
tcap_ioctl(struct tc *tc, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;

	mi_strlog(q, STRLOGIO, SL_TRACE, "-> M_IOCTL(%s)", tc_iocname(ioc->ioc_cmd));

	switch (_IOC_NR(ioc->ioc_cmd)) {
	case _IOC_NR(TCAP_IOCGOPTION):
	case _IOC_NR(TCAP_IOCSOPTION):
		mi_copyin(q, mp, NULL, sizeof(struct tcap_option));
		break;
	case _IOC_NR(TCAP_IOCGCONFIG):
	case _IOC_NR(TCAP_IOCSCONFIG):
	case _IOC_NR(TCAP_IOCTCONFIG):
	case _IOC_NR(TCAP_IOCCCONFIG):
		mi_copyin(q, mp, NULL, sizeof(struct tcap_config));
		break;
	case _IOC_NR(TCAP_IOCGSTATEM):
	case _IOC_NR(TCAP_IOCCMRESET):
		mi_copyin(q, mp, NULL, sizeof(struct tcap_statem));
		break;
	case _IOC_NR(TCAP_IOCGSTATSP):
	case _IOC_NR(TCAP_IOCSSTATSP):
	case _IOC_NR(TCAP_IOCGSTATS):
	case _IOC_NR(TCAP_IOCCSTATS):
		mi_copyin(q, mp, NULL, sizeof(struct tcap_stats));
		break;
	case _IOC_NR(TCAP_IOCGNOTIFY):
	case _IOC_NR(TCAP_IOCSNOTIFY):
	case _IOC_NR(TCAP_IOCCNOTIFY):
		mi_copyin(q, mp, NULL, sizeof(struct tcap_notify));
		break;
	case _IOC_NR(TCAP_IOCCMGMT):
		mi_copyin(q, mp, NULL, sizeof(struct tcap_mgmt));
		break;
	case _IOC_NR(TCAP_IOCCPASS):
		mi_copyin(q, mp, NULL, sizeof(struct tcap_pass));
		break;
	default:
		mi_copy_done(q, mp, EINVAL);
		break;
	}
	return (0);
}

/**
 * tcap_copyin1: - process M_IOCDATA message for M_COPYIN for TCAP
 * @tc: TCAP private structure
 * @q: active queue (upper write queue)
 * @mp: the M_IOCDATA message
 * @dp: the data copied in
 *
 * This is step 2 of the TCAP input-output control operation.  Step 2 consists of identifying the
 * object and performing a copyout for GET operations and an additional copyin for SET operations.
 */
static noinline __unlikely int
tcap_copyin1(struct tc *lm, queue_t *q, mblk_t *mp, mblk_t *dp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	int alen, len, size = -1, err = 0;
	mblk_t *bp;
	void *ptr;

	mi_strlog(q, STRLOGIO, SL_TRACE, "-> M_IOCDATA(%s)", tc_iocname(cp->cp_cmd));

	switch (_IOC_NR(cp->cp_cmd)) {
		struct df *df;
		struct tc *tc;
		struct iv *iv;
		struct op *op;
		struct dg *dg;
		struct ac *ac;
		struct tr *tr;
		struct te *te;
		struct sp *sp;
		struct sc *sc;

	case _IOC_NR(TCAP_IOCGOPTION):
	{
		struct tcap_option *arg = (typeof(arg)) dp->b_rptr;

		alen = sizeof(*arg);
		switch (arg->type) {
		case TCAP_OBJ_TYPE_DF:
			len = sizeof(struct tcap_opt_conf_df);
			if (!(df = df_lookup(lm, arg->id)))
				goto esrch;
			ptr = &df->config;
			break;
		case TCAP_OBJ_TYPE_TC:
			len = sizeof(struct tcap_opt_conf_tc);
			if (!(tc = tc_lookup(arg->id)))
				goto esrch;
			ptr = &tc->config;
			break;
		case TCAP_OBJ_TYPE_IV:
			len = sizeof(struct tcap_opt_conf_iv);
			if (!(dg = dg_lookup(lm, (arg->id >> 16))))
				goto esrch;
			if (!(iv = iv_lookup(dg, (arg->id & ((1<<16)-1)))))
				goto esrch;
			ptr = &iv->config;
			break;
		case TCAP_OBJ_TYPE_OP:
			len = sizeof(struct tcap_opt_conf_op);
			if (!(ac = ac_lookup(lm, (arg->id >>16))))
				goto esrch;
			if (!(op = op_lookup(ac, (arg->id & ((1<<16)-1)))))
				goto esrch;
			ptr = &op->config;
			break;
		case TCAP_OBJ_TYPE_DG:
			len = sizeof(struct tcap_opt_conf_dg);
			if (!(dg = dg_lookup(lm, arg->id)))
				goto esrch;
			ptr = &dg->config;
			break;
		case TCAP_OBJ_TYPE_AC:
			len = sizeof(struct tcap_opt_conf_ac);
			if (!(ac = ac_lookup(lm, arg->id)))
				goto esrch;
			ptr = &ac->config;
			break;
		case TCAP_OBJ_TYPE_TR:
			len = sizeof(struct tcap_opt_conf_tr);
			if (!(tr = tr_lookup_tid(lm, arg->id)))
				goto esrch;
			ptr = &tr->config;
			break;
		case TCAP_OBJ_TYPE_TE:
			len = sizeof(struct tcap_opt_conf_te);
			if (!(te = te_lookup(lm, arg->id)))
				goto esrch;
			ptr = &te->config;
			break;
		case TCAP_OBJ_TYPE_SP:
			len = sizeof(struct tcap_opt_conf_sp);
			if (!(sp = sp_lookup(lm, arg->id)))
				goto esrch;
			ptr = &sp->config;
			break;
		case TCAP_OBJ_TYPE_SC:
			len = sizeof(struct tcap_opt_conf_sc);
			if (!(sc = sc_lookup(arg->id)))
				goto esrch;
			ptr = &sc->config;
			break;
		default:
			goto einval;
		}
		goto copyout;
	}
	case _IOC_NR(TCAP_IOCSOPTION):
	{
		struct tcap_option *arg = (typeof(arg)) dp->b_rptr;

		alen = sizeof(*arg);
		switch (arg->type) {
		case TCAP_OBJ_TYPE_DF:
			len = sizeof(struct tcap_opt_conf_df);
			break;
		case TCAP_OBJ_TYPE_TC:
			len = sizeof(struct tcap_opt_conf_tc);
			break;
		case TCAP_OBJ_TYPE_IV:
			len = sizeof(struct tcap_opt_conf_iv);
			break;
		case TCAP_OBJ_TYPE_OP:
			len = sizeof(struct tcap_opt_conf_op);
			break;
		case TCAP_OBJ_TYPE_DG:
			len = sizeof(struct tcap_opt_conf_dg);
			break;
		case TCAP_OBJ_TYPE_AC:
			len = sizeof(struct tcap_opt_conf_ac);
			break;
		case TCAP_OBJ_TYPE_TR:
			len = sizeof(struct tcap_opt_conf_tr);
			break;
		case TCAP_OBJ_TYPE_TE:
			len = sizeof(struct tcap_opt_conf_te);
			break;
		case TCAP_OBJ_TYPE_SP:
			len = sizeof(struct tcap_opt_conf_sp);
			break;
		case TCAP_OBJ_TYPE_SC:
			len = sizeof(struct tcap_opt_conf_sc);
			break;
		default:
			goto einval;
		}
		goto copyin;
	}
	case _IOC_NR(TCAP_IOCGCONFIG):
	{
		struct tcap_config *arg = (typeof(arg)) dp->b_rptr;

		alen = sizeof(*arg);
		switch (arg->type) {
		case TCAP_OBJ_TYPE_DF:
			len = sizeof(struct tcap_conf_df);
			if (!(df = df_lookup(lm, arg->id)))
				goto esrch;
			ptr = &df->config;
			break;
		case TCAP_OBJ_TYPE_TC:
			len = sizeof(struct tcap_conf_tc);
			if (!(tc = tc_lookup(arg->id)))
				goto esrch;
			ptr = &tc->config;
			break;
		case TCAP_OBJ_TYPE_IV:
			len = sizeof(struct tcap_conf_iv);
			if (!(dg = dg_lookup(lm, (arg->id >> 16))))
				goto esrch;
			if (!(iv = iv_lookup(dg, (arg->id & ((1<<16)-1)))))
				goto esrch;
			ptr = &iv->config;
			break;
		case TCAP_OBJ_TYPE_OP:
			len = sizeof(struct tcap_conf_op);
			if (!(ac = ac_lookup(lm, (arg->id >>16))))
				goto esrch;
			if (!(op = op_lookup(ac, (arg->id & ((1<<16)-1)))))
				goto esrch;
			ptr = &op->config;
			break;
		case TCAP_OBJ_TYPE_DG:
			len = sizeof(struct tcap_conf_dg);
			if (!(dg = dg_lookup(lm, arg->id)))
				goto esrch;
			ptr = &dg->config;
			break;
		case TCAP_OBJ_TYPE_AC:
			len = sizeof(struct tcap_conf_ac);
			if (!(ac = ac_lookup(lm, arg->id)))
				goto esrch;
			ptr = &ac->config;
			break;
		case TCAP_OBJ_TYPE_TR:
			len = sizeof(struct tcap_conf_tr);
			if (!(tr = tr_lookup_tid(lm, arg->id)))
				goto esrch;
			ptr = &tr->config;
			break;
		case TCAP_OBJ_TYPE_TE:
			len = sizeof(struct tcap_conf_te);
			if (!(te = te_lookup(lm, arg->id)))
				goto esrch;
			ptr = &te->config;
			break;
		case TCAP_OBJ_TYPE_SP:
			len = sizeof(struct tcap_conf_sp);
			if (!(sp = sp_lookup(lm, arg->id)))
				goto esrch;
			ptr = &sp->config;
			break;
		case TCAP_OBJ_TYPE_SC:
			len = sizeof(struct tcap_conf_sc);
			if (!(sc = sc_lookup(arg->id)))
				goto esrch;
			ptr = &sc->config;
			break;
		default:
			goto einval;
		}
		goto copyout;
	}
	case _IOC_NR(TCAP_IOCSCONFIG):
	case _IOC_NR(TCAP_IOCTCONFIG):
	case _IOC_NR(TCAP_IOCCCONFIG):
	{
		struct tcap_config *arg = (typeof(arg)) dp->b_rptr;

		alen = sizeof(*arg);
		switch (arg->type) {
		case TCAP_OBJ_TYPE_DF:
			len = sizeof(struct tcap_conf_df);
			break;
		case TCAP_OBJ_TYPE_TC:
			len = sizeof(struct tcap_conf_tc);
			break;
		case TCAP_OBJ_TYPE_IV:
			len = sizeof(struct tcap_conf_iv);
			break;
		case TCAP_OBJ_TYPE_OP:
			len = sizeof(struct tcap_conf_op);
			break;
		case TCAP_OBJ_TYPE_DG:
			len = sizeof(struct tcap_conf_dg);
			break;
		case TCAP_OBJ_TYPE_AC:
			len = sizeof(struct tcap_conf_ac);
			break;
		case TCAP_OBJ_TYPE_TR:
			len = sizeof(struct tcap_conf_tr);
			break;
		case TCAP_OBJ_TYPE_TE:
			len = sizeof(struct tcap_conf_te);
			break;
		case TCAP_OBJ_TYPE_SP:
			len = sizeof(struct tcap_conf_sp);
			break;
		case TCAP_OBJ_TYPE_SC:
			len = sizeof(struct tcap_conf_sc);
			break;
		default:
			goto einval;
		}
		goto copyin;
	}
	case _IOC_NR(TCAP_IOCGSTATEM):
	{
		struct tcap_statem *arg = (typeof(arg)) dp->b_rptr;

		alen = sizeof(*arg);
		switch (arg->type) {
		case TCAP_OBJ_TYPE_DF:
			len = sizeof(struct tcap_statem_df);
			if (!(df = df_lookup(lm, arg->id)))
				goto esrch;
			ptr = &df->statem;
			break;
		case TCAP_OBJ_TYPE_TC:
			len = sizeof(struct tcap_statem_tc);
			if (!(tc = tc_lookup(arg->id)))
				goto esrch;
			ptr = &tc->statem;
			break;
		case TCAP_OBJ_TYPE_IV:
			len = sizeof(struct tcap_statem_iv);
			if (!(dg = dg_lookup(lm, (arg->id >> 16))))
				goto esrch;
			if (!(iv = iv_lookup(dg, (arg->id & ((1<<16)-1)))))
				goto esrch;
			ptr = &iv->statem;
			break;
		case TCAP_OBJ_TYPE_OP:
			len = sizeof(struct tcap_statem_op);
			if (!(ac = ac_lookup(lm, (arg->id >>16))))
				goto esrch;
			if (!(op = op_lookup(ac, (arg->id & ((1<<16)-1)))))
				goto esrch;
			ptr = &op->statem;
			break;
		case TCAP_OBJ_TYPE_DG:
			len = sizeof(struct tcap_statem_dg);
			if (!(dg = dg_lookup(lm, arg->id)))
				goto esrch;
			ptr = &dg->statem;
			break;
		case TCAP_OBJ_TYPE_AC:
			len = sizeof(struct tcap_statem_ac);
			if (!(ac = ac_lookup(lm, arg->id)))
				goto esrch;
			ptr = &ac->statem;
			break;
		case TCAP_OBJ_TYPE_TR:
			len = sizeof(struct tcap_statem_tr);
			if (!(tr = tr_lookup_tid(lm, arg->id)))
				goto esrch;
			ptr = &tr->statem;
			break;
		case TCAP_OBJ_TYPE_TE:
			len = sizeof(struct tcap_statem_te);
			if (!(te = te_lookup(lm, arg->id)))
				goto esrch;
			ptr = &te->statem;
			break;
		case TCAP_OBJ_TYPE_SP:
			len = sizeof(struct tcap_statem_sp);
			if (!(sp = sp_lookup(lm, arg->id)))
				goto esrch;
			ptr = &sp->statem;
			break;
		case TCAP_OBJ_TYPE_SC:
			len = sizeof(struct tcap_statem_sc);
			if (!(sc = sc_lookup(arg->id)))
				goto esrch;
			ptr = &sc->statem;
			break;
		default:
			goto einval;
		}
		goto copyout;
	}
	case _IOC_NR(TCAP_IOCCMRESET):
	{
		struct tcap_statem *arg = (typeof(arg)) dp->b_rptr;

		size = 0;
		switch (arg->type) {
		case TCAP_OBJ_TYPE_DF:
		case TCAP_OBJ_TYPE_TC:
		case TCAP_OBJ_TYPE_IV:
		case TCAP_OBJ_TYPE_OP:
		case TCAP_OBJ_TYPE_DG:
		case TCAP_OBJ_TYPE_AC:
		case TCAP_OBJ_TYPE_TR:
		case TCAP_OBJ_TYPE_TE:
		case TCAP_OBJ_TYPE_SP:
		case TCAP_OBJ_TYPE_SC:
			/* FIXME: reset the state of the object */
			err = 0;
			break;
		default:
			goto einval;
		}
		break;
	}
	case _IOC_NR(TCAP_IOCGSTATSP):
	{
		struct tcap_stats *arg = (typeof(arg)) dp->b_rptr;

		alen = sizeof(*arg);
		switch (arg->type) {
		case TCAP_OBJ_TYPE_DF:
			len = sizeof(struct tcap_stats_df);
			if (!(df = df_lookup(lm, arg->id)))
				goto esrch;
			ptr = &df->statsp;
			break;
		case TCAP_OBJ_TYPE_TC:
			len = sizeof(struct tcap_stats_tc);
			if (!(tc = tc_lookup(arg->id)))
				goto esrch;
			ptr = &tc->statsp;
			break;
		case TCAP_OBJ_TYPE_IV:
			len = sizeof(struct tcap_stats_iv);
			if (!(dg = dg_lookup(lm, (arg->id >> 16))))
				goto esrch;
			if (!(iv = iv_lookup(dg, (arg->id & ((1<<16)-1)))))
				goto esrch;
			ptr = &iv->statsp;
			break;
		case TCAP_OBJ_TYPE_OP:
			len = sizeof(struct tcap_stats_op);
			if (!(ac = ac_lookup(lm, (arg->id >>16))))
				goto esrch;
			if (!(op = op_lookup(ac, (arg->id & ((1<<16)-1)))))
				goto esrch;
			ptr = &op->statsp;
			break;
		case TCAP_OBJ_TYPE_DG:
			len = sizeof(struct tcap_stats_dg);
			if (!(dg = dg_lookup(lm, arg->id)))
				goto esrch;
			ptr = &dg->statsp;
			break;
		case TCAP_OBJ_TYPE_AC:
			len = sizeof(struct tcap_stats_ac);
			if (!(ac = ac_lookup(lm, arg->id)))
				goto esrch;
			ptr = &ac->statsp;
			break;
		case TCAP_OBJ_TYPE_TR:
			len = sizeof(struct tcap_stats_tr);
			if (!(tr = tr_lookup_tid(lm, arg->id)))
				goto esrch;
			ptr = &tr->statsp;
			break;
		case TCAP_OBJ_TYPE_TE:
			len = sizeof(struct tcap_stats_te);
			if (!(te = te_lookup(lm, arg->id)))
				goto esrch;
			ptr = &te->statsp;
			break;
		case TCAP_OBJ_TYPE_SP:
			len = sizeof(struct tcap_stats_sp);
			if (!(sp = sp_lookup(lm, arg->id)))
				goto esrch;
			ptr = &sp->statsp;
			break;
		case TCAP_OBJ_TYPE_SC:
			len = sizeof(struct tcap_stats_sc);
			if (!(sc = sc_lookup(arg->id)))
				goto esrch;
			ptr = &sc->statsp;
			break;
		default:
			goto einval;
		}
		goto copyout;
	}
	case _IOC_NR(TCAP_IOCSSTATSP):
	{
		struct tcap_stats *arg = (typeof(arg)) dp->b_rptr;

		alen = sizeof(*arg);
		switch (arg->type) {
		case TCAP_OBJ_TYPE_DF:
			len = sizeof(struct tcap_stats_df);
			break;
		case TCAP_OBJ_TYPE_TC:
			len = sizeof(struct tcap_stats_tc);
			break;
		case TCAP_OBJ_TYPE_IV:
			len = sizeof(struct tcap_stats_iv);
			break;
		case TCAP_OBJ_TYPE_OP:
			len = sizeof(struct tcap_stats_op);
			break;
		case TCAP_OBJ_TYPE_DG:
			len = sizeof(struct tcap_stats_dg);
			break;
		case TCAP_OBJ_TYPE_AC:
			len = sizeof(struct tcap_stats_ac);
			break;
		case TCAP_OBJ_TYPE_TR:
			len = sizeof(struct tcap_stats_tr);
			break;
		case TCAP_OBJ_TYPE_TE:
			len = sizeof(struct tcap_stats_te);
			break;
		case TCAP_OBJ_TYPE_SP:
			len = sizeof(struct tcap_stats_sp);
			break;
		case TCAP_OBJ_TYPE_SC:
			len = sizeof(struct tcap_stats_sc);
			break;
		default:
			goto einval;
		}
		goto copyin;
	}
	case _IOC_NR(TCAP_IOCGSTATS):
	{
		struct tcap_stats *arg = (typeof(arg)) dp->b_rptr;

		alen = sizeof(*arg);
		switch (arg->type) {
		case TCAP_OBJ_TYPE_DF:
			len = sizeof(struct tcap_stats_df);
			if (!(df = df_lookup(lm, arg->id)))
				goto esrch;
			ptr = &df->stats;
			break;
		case TCAP_OBJ_TYPE_TC:
			len = sizeof(struct tcap_stats_tc);
			if (!(tc = tc_lookup(arg->id)))
				goto esrch;
			ptr = &tc->stats;
			break;
		case TCAP_OBJ_TYPE_IV:
			len = sizeof(struct tcap_stats_iv);
			if (!(dg = dg_lookup(lm, (arg->id >> 16))))
				goto esrch;
			if (!(iv = iv_lookup(dg, (arg->id & ((1<<16)-1)))))
				goto esrch;
			ptr = &iv->stats;
			break;
		case TCAP_OBJ_TYPE_OP:
			len = sizeof(struct tcap_stats_op);
			if (!(ac = ac_lookup(lm, (arg->id >>16))))
				goto esrch;
			if (!(op = op_lookup(ac, (arg->id & ((1<<16)-1)))))
				goto esrch;
			ptr = &op->stats;
			break;
		case TCAP_OBJ_TYPE_DG:
			len = sizeof(struct tcap_stats_dg);
			if (!(dg = dg_lookup(lm, arg->id)))
				goto esrch;
			ptr = &dg->stats;
			break;
		case TCAP_OBJ_TYPE_AC:
			len = sizeof(struct tcap_stats_ac);
			if (!(ac = ac_lookup(lm, arg->id)))
				goto esrch;
			ptr = &ac->stats;
			break;
		case TCAP_OBJ_TYPE_TR:
			len = sizeof(struct tcap_stats_tr);
			if (!(tr = tr_lookup_tid(lm, arg->id)))
				goto esrch;
			ptr = &tr->stats;
			break;
		case TCAP_OBJ_TYPE_TE:
			len = sizeof(struct tcap_stats_te);
			if (!(te = te_lookup(lm, arg->id)))
				goto esrch;
			ptr = &te->stats;
			break;
		case TCAP_OBJ_TYPE_SP:
			len = sizeof(struct tcap_stats_sp);
			if (!(sp = sp_lookup(lm, arg->id)))
				goto esrch;
			ptr = &sp->stats;
			break;
		case TCAP_OBJ_TYPE_SC:
			len = sizeof(struct tcap_stats_sc);
			if (!(sc = sc_lookup(arg->id)))
				goto esrch;
			ptr = &sc->stats;
			break;
		default:
			goto einval;
		}
		goto copyout;
	}
	case _IOC_NR(TCAP_IOCCSTATS):
	{
		struct tcap_stats *arg = (typeof(arg)) dp->b_rptr;

		alen = sizeof(*arg);
		switch (arg->type) {
		case TCAP_OBJ_TYPE_DF:
			len = sizeof(struct tcap_stats_df);
			if (!(df = df_lookup(lm, arg->id)))
				goto esrch;
			ptr = &df->stats;
			break;
		case TCAP_OBJ_TYPE_TC:
			len = sizeof(struct tcap_stats_tc);
			if (!(tc = tc_lookup(arg->id)))
				goto esrch;
			ptr = &tc->stats;
			break;
		case TCAP_OBJ_TYPE_IV:
			len = sizeof(struct tcap_stats_iv);
			if (!(dg = dg_lookup(lm, (arg->id >> 16))))
				goto esrch;
			if (!(iv = iv_lookup(dg, (arg->id & ((1<<16)-1)))))
				goto esrch;
			ptr = &iv->stats;
			break;
		case TCAP_OBJ_TYPE_OP:
			len = sizeof(struct tcap_stats_op);
			if (!(ac = ac_lookup(lm, (arg->id >>16))))
				goto esrch;
			if (!(op = op_lookup(ac, (arg->id & ((1<<16)-1)))))
				goto esrch;
			ptr = &op->stats;
			break;
		case TCAP_OBJ_TYPE_DG:
			len = sizeof(struct tcap_stats_dg);
			if (!(dg = dg_lookup(lm, arg->id)))
				goto esrch;
			ptr = &dg->stats;
			break;
		case TCAP_OBJ_TYPE_AC:
			len = sizeof(struct tcap_stats_ac);
			if (!(ac = ac_lookup(lm, arg->id)))
				goto esrch;
			ptr = &ac->stats;
			break;
		case TCAP_OBJ_TYPE_TR:
			len = sizeof(struct tcap_stats_tr);
			if (!(tr = tr_lookup_tid(lm, arg->id)))
				goto esrch;
			ptr = &tr->stats;
			break;
		case TCAP_OBJ_TYPE_TE:
			len = sizeof(struct tcap_stats_te);
			if (!(te = te_lookup(lm, arg->id)))
				goto esrch;
			ptr = &te->stats;
			break;
		case TCAP_OBJ_TYPE_SP:
			len = sizeof(struct tcap_stats_sp);
			if (!(sp = sp_lookup(lm, arg->id)))
				goto esrch;
			ptr = &sp->stats;
			break;
		case TCAP_OBJ_TYPE_SC:
			len = sizeof(struct tcap_stats_sc);
			if (!(sc = sc_lookup(arg->id)))
				goto esrch;
			ptr = &sc->stats;
			break;
		default:
			goto einval;
		}
		goto copyout_clear;
	}
	case _IOC_NR(TCAP_IOCGNOTIFY):
	{
		struct tcap_notify *arg = (typeof(arg)) dp->b_rptr;

		alen = sizeof(*arg);
		switch (arg->type) {
		case TCAP_OBJ_TYPE_DF:
			len = sizeof(struct tcap_notify_df);
			if (!(df = df_lookup(lm, arg->id)))
				goto esrch;
			ptr = &df->notify;
			break;
		case TCAP_OBJ_TYPE_TC:
			len = sizeof(struct tcap_notify_tc);
			if (!(tc = tc_lookup(arg->id)))
				goto esrch;
			ptr = &tc->notify;
			break;
		case TCAP_OBJ_TYPE_IV:
			len = sizeof(struct tcap_notify_iv);
			if (!(dg = dg_lookup(lm, (arg->id >> 16))))
				goto esrch;
			if (!(iv = iv_lookup(dg, (arg->id & ((1<<16)-1)))))
				goto esrch;
			ptr = &iv->notify;
			break;
		case TCAP_OBJ_TYPE_OP:
			len = sizeof(struct tcap_notify_op);
			if (!(ac = ac_lookup(lm, (arg->id >>16))))
				goto esrch;
			if (!(op = op_lookup(ac, (arg->id & ((1<<16)-1)))))
				goto esrch;
			ptr = &op->notify;
			break;
		case TCAP_OBJ_TYPE_DG:
			len = sizeof(struct tcap_notify_dg);
			if (!(dg = dg_lookup(lm, arg->id)))
				goto esrch;
			ptr = &dg->notify;
			break;
		case TCAP_OBJ_TYPE_AC:
			len = sizeof(struct tcap_notify_ac);
			if (!(ac = ac_lookup(lm, arg->id)))
				goto esrch;
			ptr = &ac->notify;
			break;
		case TCAP_OBJ_TYPE_TR:
			len = sizeof(struct tcap_notify_tr);
			if (!(tr = tr_lookup_tid(lm, arg->id)))
				goto esrch;
			ptr = &tr->notify;
			break;
		case TCAP_OBJ_TYPE_TE:
			len = sizeof(struct tcap_notify_te);
			if (!(te = te_lookup(lm, arg->id)))
				goto esrch;
			ptr = &te->notify;
			break;
		case TCAP_OBJ_TYPE_SP:
			len = sizeof(struct tcap_notify_sp);
			if (!(sp = sp_lookup(lm, arg->id)))
				goto esrch;
			ptr = &sp->notify;
			break;
		case TCAP_OBJ_TYPE_SC:
			len = sizeof(struct tcap_notify_sc);
			if (!(sc = sc_lookup(arg->id)))
				goto esrch;
			ptr = &sc->notify;
			break;
		default:
			goto einval;
		}
		goto copyout;
	}
	case _IOC_NR(TCAP_IOCSNOTIFY):
	case _IOC_NR(TCAP_IOCCNOTIFY):
	{
		struct tcap_notify *arg = (typeof(arg)) dp->b_rptr;

		alen = sizeof(*arg);
		switch (arg->type) {
		case TCAP_OBJ_TYPE_DF:
			len = sizeof(struct tcap_notify_df);
			break;
		case TCAP_OBJ_TYPE_TC:
			len = sizeof(struct tcap_notify_tc);
			break;
		case TCAP_OBJ_TYPE_IV:
			len = sizeof(struct tcap_notify_iv);
			break;
		case TCAP_OBJ_TYPE_OP:
			len = sizeof(struct tcap_notify_op);
			break;
		case TCAP_OBJ_TYPE_DG:
			len = sizeof(struct tcap_notify_dg);
			break;
		case TCAP_OBJ_TYPE_AC:
			len = sizeof(struct tcap_notify_ac);
			break;
		case TCAP_OBJ_TYPE_TR:
			len = sizeof(struct tcap_notify_tr);
			break;
		case TCAP_OBJ_TYPE_TE:
			len = sizeof(struct tcap_notify_te);
			break;
		case TCAP_OBJ_TYPE_SP:
			len = sizeof(struct tcap_notify_sp);
			break;
		case TCAP_OBJ_TYPE_SC:
			len = sizeof(struct tcap_notify_sc);
			break;
		default:
			goto einval;
		}
		goto copyin;
	}
	case _IOC_NR(TCAP_IOCCMGMT):
	{
		struct tcap_mgmt *arg = (typeof(arg)) dp->b_rptr;

		size = 0;
		switch (arg->type) {
		case TCAP_OBJ_TYPE_DF:
		case TCAP_OBJ_TYPE_TC:
		case TCAP_OBJ_TYPE_IV:
		case TCAP_OBJ_TYPE_OP:
		case TCAP_OBJ_TYPE_DG:
		case TCAP_OBJ_TYPE_AC:
		case TCAP_OBJ_TYPE_TR:
		case TCAP_OBJ_TYPE_TE:
		case TCAP_OBJ_TYPE_SP:
		case TCAP_OBJ_TYPE_SC:
			/* FIXME: perform command on object */
			err = 0;
			break;
		default:
			goto einval;
		}
		break;
	}
	case _IOC_NR(TCAP_IOCCPASS):
	{
		struct tcap_pass *arg = (typeof(arg)) dp->b_rptr;

		alen = sizeof(*arg);
		len = 0;
		goto copyin;
	}
	default:
	      einval:
		err = EINVAL;
		break;
	      esrch:
		err = ESRCH;
		break;
	      enobufs:
		err = ENOBUFS;
		break;
	      copyin:
		if (len == 0) {
			err = EINVAL;
			break;
		}
		size = alen + len;
		break;
	      copyout:
		if (len == 0 || ptr == NULL) {
			err = EINVAL;
			break;
		}
		if (!(bp = mi_copyout_alloc(q, mp, NULL, alen + len, false)))
			goto enobufs;
		bcopy(dp->b_rptr, bp->b_rptr, alen);
		bcopy(ptr, bp->b_rptr + alen, len);
		break;
	      copyout_clear:
		if (len == 0 || ptr == NULL) {
			err = EINVAL;
			break;
		}
		if (!(bp = mi_copyout_alloc(q, mp, NULL, alen + len, false)))
			goto enobufs;
		bcopy(dp->b_rptr, bp->b_rptr, alen);
		bcopy(ptr, bp->b_rptr + alen, len);
		bzero(ptr, len);
		break;
	}
	if (err < 0)
		return (err);
	if (err > 0)
		mi_copy_done(q, mp, err);
	if (err == 0) {
		if (size == 0)
			mi_copy_done(q, mp, 0);
		else if (size == -1)
			mi_copyout(q, mp);
		else
			mi_copyin(q, mp, NULL, size);
	}
	return (0);
}

/**
 * tcap_copyin2: - process M_IOCDATA message for M_COPYIN for TCAP
 * @lm: TCAP private structure
 * @q: active queue (upper write queue)
 * @mp: the M_IOCDATA message
 * @dp: the data copied in
 *
 * This is step 3 for SET operations.  This is the result of the implicit or explicit copyin
 * operation for the object specific structure.  We can now perform sets and commits.  All SET
 * operations also include a last copyout step that copies out the information actually set.
 */
static noinline __unlikely int
tcap_copyin2(struct tc *lm, queue_t *q, mblk_t *mp, mblk_t *dp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	int len, err = 0;
	mblk_t *bp;

	mi_strlog(q, STRLOGIO, SL_TRACE, "-> M_IOCDATA(%s)", tc_iocname(cp->cp_cmd));

	len = dp->b_wptr - dp->b_rptr;
	if (!(bp = mi_copyout_alloc(q, mp, NULL, len, false)))
		goto enobufs;
	bcopy(dp->b_rptr, bp->b_rptr, len);

	switch (_IOC_NR(cp->cp_cmd)) {
		struct df *df;
		struct tc *tc;
		struct iv *iv;
		struct op *op;
		struct dg *dg;
		struct ac *ac;
		struct te *te;
		struct tr *tr;
		struct sp *sp;
		struct sc *sc;

	case _IOC_NR(TCAP_IOCSOPTION):
	{
		struct tcap_option *arg = (typeof(arg)) dp->b_rptr;

		switch (arg->type) {
		case TCAP_OBJ_TYPE_DF:
			if (!(df = df_lookup(lm, arg->id)))
				goto esrch;
			df->config = arg->options[0].df;
			break;
		case TCAP_OBJ_TYPE_TC:
			if (!(tc = tc_lookup(arg->id)))
				goto esrch;
			tc->config = arg->options[0].tc;
			break;
		case TCAP_OBJ_TYPE_IV:
			if (!(dg = dg_lookup(lm, (arg->id >> 16))))
				goto esrch;
			if (!(iv = iv_lookup(dg, (arg->id & ((1<<16)-1)))))
				goto esrch;
			iv->config = arg->options[0].iv;
			break;
		case TCAP_OBJ_TYPE_OP:
			if (!(ac = ac_lookup(lm, (arg->id >>16))))
				goto esrch;
			if (!(op = op_lookup(ac, (arg->id & ((1<<16)-1)))))
				goto esrch;
			op->config = arg->options[0].op;
			break;
		case TCAP_OBJ_TYPE_DG:
			if (!(dg = dg_lookup(lm, arg->id)))
				goto esrch;
			dg->config = arg->options[0].dg;
			break;
		case TCAP_OBJ_TYPE_AC:
			if (!(ac = ac_lookup(lm, arg->id)))
				goto esrch;
			ac->config = arg->options[0].ac;
			break;
		case TCAP_OBJ_TYPE_TR:
			if (!(tr = tr_lookup_tid(lm, arg->id)))
				goto esrch;
			tr->config = arg->options[0].tr;
			break;
		case TCAP_OBJ_TYPE_TE:
			if (!(te = te_lookup(lm, arg->id)))
				goto esrch;
			te->config = arg->options[0].te;
			break;
		case TCAP_OBJ_TYPE_SP:
			if (!(sp = sp_lookup(lm, arg->id)))
				goto esrch;
			sp->config = arg->options[0].sp;
			break;
		case TCAP_OBJ_TYPE_SC:
			if (!(sc = sc_lookup(arg->id)))
				goto esrch;
			sc->config = arg->options[0].sc;
			break;
		default:
			goto einval;
		}
		break;
	}
	case _IOC_NR(TCAP_IOCSCONFIG):
	{
		struct tcap_config *arg = (typeof(arg)) dp->b_rptr;

		switch (arg->cmd) {
		case TCAP_ADD:
			switch (arg->type) {
			case TCAP_OBJ_TYPE_DF:
			case TCAP_OBJ_TYPE_TC:
			case TCAP_OBJ_TYPE_IV:
			case TCAP_OBJ_TYPE_OP:
			case TCAP_OBJ_TYPE_DG:
			case TCAP_OBJ_TYPE_AC:
			case TCAP_OBJ_TYPE_TR:
			case TCAP_OBJ_TYPE_TE:
			case TCAP_OBJ_TYPE_SP:
			case TCAP_OBJ_TYPE_SC:
			default:
				goto einval;
			}
			break;
		case TCAP_CHA:
			switch (arg->type) {
			case TCAP_OBJ_TYPE_DF:
			case TCAP_OBJ_TYPE_TC:
			case TCAP_OBJ_TYPE_IV:
			case TCAP_OBJ_TYPE_OP:
			case TCAP_OBJ_TYPE_DG:
			case TCAP_OBJ_TYPE_AC:
			case TCAP_OBJ_TYPE_TR:
			case TCAP_OBJ_TYPE_TE:
			case TCAP_OBJ_TYPE_SP:
			case TCAP_OBJ_TYPE_SC:
			default:
				goto einval;
			}
			break;
		case TCAP_DEL:
			switch (arg->type) {
			case TCAP_OBJ_TYPE_DF:
			case TCAP_OBJ_TYPE_TC:
			case TCAP_OBJ_TYPE_IV:
			case TCAP_OBJ_TYPE_OP:
			case TCAP_OBJ_TYPE_DG:
			case TCAP_OBJ_TYPE_AC:
			case TCAP_OBJ_TYPE_TR:
			case TCAP_OBJ_TYPE_TE:
			case TCAP_OBJ_TYPE_SP:
			case TCAP_OBJ_TYPE_SC:
			default:
				goto einval;
			}
			break;
		case TCAP_GET:
		default:
			goto einval;
		}
		break;
	}
	case _IOC_NR(TCAP_IOCTCONFIG):
	{
		struct tcap_config *arg = (typeof(arg)) dp->b_rptr;

		break;
	}
	case _IOC_NR(TCAP_IOCCCONFIG):
	{
		struct tcap_config *arg = (typeof(arg)) dp->b_rptr;

		break;
	}
	case _IOC_NR(TCAP_IOCSSTATSP):
	{
		struct tcap_stats *arg = (typeof(arg)) dp->b_rptr;

		switch (arg->type) {
		case TCAP_OBJ_TYPE_DF:
			if (!(df = df_lookup(lm, arg->id)))
				goto esrch;
			df->statsp = arg->stats[0].df;
			break;
		case TCAP_OBJ_TYPE_TC:
			if (!(tc = tc_lookup(arg->id)))
				goto esrch;
			tc->statsp = arg->stats[0].tc;
			break;
		case TCAP_OBJ_TYPE_IV:
			if (!(dg = dg_lookup(lm, (arg->id >> 16))))
				goto esrch;
			if (!(iv = iv_lookup(dg, (arg->id & ((1<<16)-1)))))
				goto esrch;
			iv->statsp = arg->stats[0].iv;
			break;
		case TCAP_OBJ_TYPE_OP:
			if (!(ac = ac_lookup(lm, (arg->id >>16))))
				goto esrch;
			if (!(op = op_lookup(ac, (arg->id & ((1<<16)-1)))))
				goto esrch;
			op->statsp = arg->stats[0].op;
			break;
		case TCAP_OBJ_TYPE_DG:
			if (!(dg = dg_lookup(lm, arg->id)))
				goto esrch;
			dg->statsp = arg->stats[0].dg;
			break;
		case TCAP_OBJ_TYPE_AC:
			if (!(ac = ac_lookup(lm, arg->id)))
				goto esrch;
			ac->statsp = arg->stats[0].ac;
			break;
		case TCAP_OBJ_TYPE_TR:
			if (!(tr = tr_lookup_tid(lm, arg->id)))
				goto esrch;
			tr->statsp = arg->stats[0].tr;
			break;
		case TCAP_OBJ_TYPE_TE:
			if (!(te = te_lookup(lm, arg->id)))
				goto esrch;
			te->statsp = arg->stats[0].te;
			break;
		case TCAP_OBJ_TYPE_SP:
			if (!(sp = sp_lookup(lm, arg->id)))
				goto esrch;
			sp->statsp = arg->stats[0].sp;
			break;
		case TCAP_OBJ_TYPE_SC:
			if (!(sc = sc_lookup(arg->id)))
				goto esrch;
			sc->statsp = arg->stats[0].sc;
			break;
		default:
			goto einval;
		}
		break;
	}
	case _IOC_NR(TCAP_IOCSNOTIFY):
	{
		struct tcap_notify *arg = (typeof(arg)) dp->b_rptr;

		switch (arg->type) {
		case TCAP_OBJ_TYPE_DF:
			if (!(df = df_lookup(lm, arg->id)))
				goto esrch;
			arg->notify[0].df.events |= df->notify.events;
			df->notify = arg->notify[0].df;
			break;
		case TCAP_OBJ_TYPE_TC:
			if (!(tc = tc_lookup(arg->id)))
				goto esrch;
			arg->notify[0].tc.events |= tc->notify.events;
			tc->notify = arg->notify[0].tc;
			break;
		case TCAP_OBJ_TYPE_IV:
			if (!(dg = dg_lookup(lm, (arg->id >> 16))))
				goto esrch;
			if (!(iv = iv_lookup(dg, (arg->id & ((1<<16)-1)))))
				goto esrch;
			arg->notify[0].iv.events |= iv->notify.events;
			iv->notify = arg->notify[0].iv;
			break;
		case TCAP_OBJ_TYPE_OP:
			if (!(ac = ac_lookup(lm, (arg->id >>16))))
				goto esrch;
			if (!(op = op_lookup(ac, (arg->id & ((1<<16)-1)))))
				goto esrch;
			arg->notify[0].op.events |= op->notify.events;
			op->notify = arg->notify[0].op;
			break;
		case TCAP_OBJ_TYPE_DG:
			if (!(dg = dg_lookup(lm, arg->id)))
				goto esrch;
			arg->notify[0].dg.events |= dg->notify.events;
			dg->notify = arg->notify[0].dg;
			break;
		case TCAP_OBJ_TYPE_AC:
			if (!(ac = ac_lookup(lm, arg->id)))
				goto esrch;
			arg->notify[0].ac.events |= ac->notify.events;
			ac->notify = arg->notify[0].ac;
			break;
		case TCAP_OBJ_TYPE_TR:
			if (!(tr = tr_lookup_tid(lm, arg->id)))
				goto esrch;
			arg->notify[0].tr.events |= tr->notify.events;
			tr->notify = arg->notify[0].tr;
			break;
		case TCAP_OBJ_TYPE_TE:
			if (!(te = te_lookup(lm, arg->id)))
				goto esrch;
			arg->notify[0].te.events |= te->notify.events;
			te->notify = arg->notify[0].te;
			break;
		case TCAP_OBJ_TYPE_SP:
			if (!(sp = sp_lookup(lm, arg->id)))
				goto esrch;
			arg->notify[0].sp.events |= sp->notify.events;
			sp->notify = arg->notify[0].sp;
			break;
		case TCAP_OBJ_TYPE_SC:
			if (!(sc = sc_lookup(arg->id)))
				goto esrch;
			arg->notify[0].sc.events |= sc->notify.events;
			sc->notify = arg->notify[0].sc;
			break;
		default:
			goto einval;
		}
		break;
	}
	case _IOC_NR(TCAP_IOCCNOTIFY):
	{
		struct tcap_notify *arg = (typeof(arg)) dp->b_rptr;

		switch (arg->type) {
		case TCAP_OBJ_TYPE_DF:
			if (!(df = df_lookup(lm, arg->id)))
				goto esrch;
			arg->notify[0].df.events = df->notify.events & ~arg->notify[0].df.events;
			df->notify = arg->notify[0].df;
			break;
		case TCAP_OBJ_TYPE_TC:
			if (!(tc = tc_lookup(arg->id)))
				goto esrch;
			arg->notify[0].tc.events = tc->notify.events & ~arg->notify[0].tc.events;
			tc->notify = arg->notify[0].tc;
			break;
		case TCAP_OBJ_TYPE_IV:
			if (!(dg = dg_lookup(lm, (arg->id >> 16))))
				goto esrch;
			if (!(iv = iv_lookup(dg, (arg->id & ((1<<16)-1)))))
				goto esrch;
			arg->notify[0].iv.events = iv->notify.events & ~arg->notify[0].iv.events;
			iv->notify = arg->notify[0].iv;
			break;
		case TCAP_OBJ_TYPE_OP:
			if (!(ac = ac_lookup(lm, (arg->id >>16))))
				goto esrch;
			if (!(op = op_lookup(ac, (arg->id & ((1<<16)-1)))))
				goto esrch;
			arg->notify[0].op.events = op->notify.events & ~arg->notify[0].op.events;
			op->notify = arg->notify[0].op;
			break;
		case TCAP_OBJ_TYPE_DG:
			if (!(dg = dg_lookup(lm, arg->id)))
				goto esrch;
			arg->notify[0].dg.events = dg->notify.events & ~arg->notify[0].dg.events;
			dg->notify = arg->notify[0].dg;
			break;
		case TCAP_OBJ_TYPE_AC:
			if (!(ac = ac_lookup(lm, arg->id)))
				goto esrch;
			arg->notify[0].ac.events = ac->notify.events & ~arg->notify[0].ac.events;
			ac->notify = arg->notify[0].ac;
			break;
		case TCAP_OBJ_TYPE_TR:
			if (!(tr = tr_lookup_tid(lm, arg->id)))
				goto esrch;
			arg->notify[0].tr.events = tr->notify.events & ~arg->notify[0].tr.events;
			tr->notify = arg->notify[0].tr;
			break;
		case TCAP_OBJ_TYPE_TE:
			if (!(te = te_lookup(lm, arg->id)))
				goto esrch;
			arg->notify[0].te.events = te->notify.events & ~arg->notify[0].te.events;
			te->notify = arg->notify[0].te;
			break;
		case TCAP_OBJ_TYPE_SP:
			if (!(sp = sp_lookup(lm, arg->id)))
				goto esrch;
			arg->notify[0].sp.events = sp->notify.events & ~arg->notify[0].sp.events;
			sp->notify = arg->notify[0].sp;
			break;
		case TCAP_OBJ_TYPE_SC:
			if (!(sc = sc_lookup(arg->id)))
				goto esrch;
			arg->notify[0].sc.events = sc->notify.events & ~arg->notify[0].sc.events;
			sc->notify = arg->notify[0].sc;
			break;
		default:
			goto einval;
		}
		break;
	}
	default:
		err = EPROTO;
		break;
	      einval:
		err = EINVAL;
		break;
	      enobufs:
		err = ENOBUFS;
		break;
	      esrch:
		err = ESRCH;
		break;
	}
	if (err < 0)
		return (err);
	if (err > 0)
		mi_copy_done(q, mp, err);
	if (err == 0)
		mi_copyout(q, mp);
	return (0);
}

/**
 * tcap_copyout: - process M_IOCDATA message for M_COPYOUT for TCAP
 * @lm: TCAP private structure
 * @q: active queue (upper write queue)
 * @mp: the M_IOCDATA message
 */
static noinline __unlikely int
tcap_copyout(struct tc *lm, queue_t *q, mblk_t *mp)
{
	mi_copyout(q, mp);
	return (0);
}

/*
 *  =========================================================================
 *
 *  STREAMS Message Handling
 *
 *  =========================================================================
 */

static int
tcap_w_proto_tci(struct tc *tc, queue_t *q, mblk_t *mp)
{
	t_uscalar_t oldstate = tcap_get_c_state(tc);
	t_uscalar_t prim;
	int rtn;

	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(prim)))
		goto badprim;

	prim = *(typeof(prim) *) mp->b_rptr;

	mi_strlog(q, STRLOGRX, SL_TRACE, "-> %s", tc_primname(prim));

	switch (prim) {
	case TC_INFO_REQ:
		rtn = tc_info_req(tc, q, mp);
		break;
	case TC_BIND_REQ:
		rtn = tc_bind_req(tc, q, mp);
		break;
	case TC_UNBIND_REQ:
		rtn = tc_unbind_req(tc, q, mp);
		break;
	case TC_SUBS_BIND_REQ:
		rtn = tc_subs_bind_req(tc, q, mp);
		break;
	case TC_SUBS_UNBIND_REQ:
		rtn = tc_subs_unbind_req(tc, q, mp);
		break;
	case TC_OPTMGMT_REQ:
		rtn = tc_optmgmt_req(tc, q, mp);
		break;
	case TC_UNI_REQ:
		rtn = tc_uni_req(tc, q, mp);
		break;
	case TC_BEGIN_REQ:
		rtn = tc_begin_req(tc, q, mp);
		break;
	case TC_BEGIN_RES:
		rtn = tc_begin_res(tc, q, mp);
		break;
	case TC_CONT_REQ:
		rtn = tc_cont_req(tc, q, mp);
		break;
	case TC_END_REQ:
		rtn = tc_end_req(tc, q, mp);
		break;
	case TC_ABORT_REQ:
		rtn = tc_abort_req(tc, q, mp);
		break;
	case TC_INVOKE_REQ:
		rtn = tc_invoke_req(tc, q, mp);
		break;
	case TC_RESULT_REQ:
		rtn = tc_result_req(tc, q, mp);
		break;
	case TC_ERROR_REQ:
		rtn = tc_error_req(tc, q, mp);
		break;
	case TC_CANCEL_REQ:
		rtn = tc_cancel_req(tc, q, mp);
		break;
	case TC_REJECT_REQ:
		rtn = tc_reject_req(tc, q, mp);
		break;
	default:
		rtn = tc_other_req(tc, q, mp);
		break;
	}
	if (rtn < 0)
		tcap_set_c_state(tc, oldstate);
	return (rtn);
      badprim:
	mi_strlog(q, 0, SL_ERROR, "%s: bad primitive", __FUNCTION__);
	freemsg(mp);
	return (0);

}

static int
tcap_w_proto_tri(struct tc *tc, queue_t *q, mblk_t *mp)
{
	t_uscalar_t oldstate = tcap_get_r_state(tc);
	int rtn;

	switch (*(t_uscalar_t *) mp->b_rptr) {
	case TR_INFO_REQ:
		printd(("%s: %p: -> TR_INFO_REQ\n", DRV_NAME, tc));
		rtn = tr_info_req(tc, q, mp);
		break;
	case TR_BIND_REQ:
		printd(("%s: %p: -> TR_BIND_REQ\n", DRV_NAME, tc));
		rtn = tr_bind_req(tc, q, mp);
		break;
	case TR_UNBIND_REQ:
		printd(("%s: %p: -> TR_UNBIND_REQ\n", DRV_NAME, tc));
		rtn = tr_unbind_req(tc, q, mp);
		break;
	case TR_OPTMGMT_REQ:
		printd(("%s: %p: -> TR_OPTMGMT_REQ\n", DRV_NAME, tc));
		rtn = tr_optmgmt_req(tc, q, mp);
		break;
	case TR_UNI_REQ:
		printd(("%s: %p: -> TR_UNI_REQ\n", DRV_NAME, tc));
		rtn = tr_uni_req(tc, q, mp);
		break;
	case TR_BEGIN_REQ:
		printd(("%s: %p: -> TR_BEGIN_REQ\n", DRV_NAME, tc));
		rtn = tr_begin_req(tc, q, mp);
		break;
	case TR_BEGIN_RES:
		printd(("%s: %p: -> TR_BEGIN_RES\n", DRV_NAME, tc));
		rtn = tr_begin_res(tc, q, mp);
		break;
	case TR_CONT_REQ:
		printd(("%s: %p: -> TR_CONT_REQ\n", DRV_NAME, tc));
		rtn = tr_cont_req(tc, q, mp);
		break;
	case TR_END_REQ:
		printd(("%s: %p: -> TR_END_REQ\n", DRV_NAME, tc));
		rtn = tr_end_req(tc, q, mp);
		break;
	case TR_ABORT_REQ:
		printd(("%s: %p: -> TR_ABORT_REQ\n", DRV_NAME, tc));
		rtn = tr_abort_req(tc, q, mp);
		break;
	default:
		printd(("%s: %p: -> ????\n", DRV_NAME, tc));
		rtn = tr_other_req(tc, q, mp);
		break;
	}
	if (rtn < 0)
		tcap_set_r_state(tc, oldstate);
	return (rtn);
}

static int
tcap_w_proto_tpi(struct tc *tc, queue_t *q, mblk_t *mp)
{
	t_uscalar_t oldstate = tcap_get_t_state(tc);
	int rtn;

	switch (*(t_uscalar_t *) mp->b_rptr) {
	case T_CONN_REQ:
		printd(("%s: %p: -> T_CONN_REQ\n", DRV_NAME, tc));
		rtn = t_conn_req(tc, q, mp);
		break;
	case T_CONN_RES:
		printd(("%s: %p: -> T_CONN_RES\n", DRV_NAME, tc));
		rtn = t_conn_res(tc, q, mp);
		break;
	case T_DISCON_REQ:
		printd(("%s: %p: -> T_DISCON_REQ\n", DRV_NAME, tc));
		rtn = t_discon_req(tc, q, mp);
		break;
	case T_DATA_REQ:
		printd(("%s: %p: -> T_DATA_REQ\n", DRV_NAME, tc));
		rtn = t_data_req(tc, q, mp);
		break;
	case T_EXDATA_REQ:
		printd(("%s: %p: -> T_EXDATA_REQ\n", DRV_NAME, tc));
		rtn = t_exdata_req(tc, q, mp);
		break;
	case T_INFO_REQ:
		printd(("%s: %p: -> T_INFO_REQ\n", DRV_NAME, tc));
		rtn = t_info_req(tc, q, mp);
		break;
	case T_BIND_REQ:
		printd(("%s: %p: -> T_BIND_REQ\n", DRV_NAME, tc));
		rtn = t_bind_req(tc, q, mp);
		break;
	case T_UNBIND_REQ:
		printd(("%s: %p: -> T_UNBIND_REQ\n", DRV_NAME, tc));
		rtn = t_unbind_req(tc, q, mp);
		break;
	case T_UNITDATA_REQ:
		printd(("%s: %p: -> T_UNITDATA_REQ\n", DRV_NAME, tc));
		rtn = t_unitdata_req(tc, q, mp);
		break;
	case T_OPTMGMT_REQ:
		printd(("%s: %p: -> T_OPTMGMT_REQ\n", DRV_NAME, tc));
		rtn = t_optmgmt_req(tc, q, mp);
		break;
	case T_ORDREL_REQ:
		printd(("%s: %p: -> T_ORDREL_REQ\n", DRV_NAME, tc));
		rtn = t_ordrel_req(tc, q, mp);
		break;
	case T_OPTDATA_REQ:
		printd(("%s: %p: -> T_OPTDATA_REQ\n", DRV_NAME, tc));
		rtn = t_optdata_req(tc, q, mp);
		break;
	case T_ADDR_REQ:
		printd(("%s: %p: -> T_ADDR_REQ\n", DRV_NAME, tc));
		rtn = t_addr_req(tc, q, mp);
		break;
	case T_CAPABILITY_REQ:
		printd(("%s: %p: -> T_CAPABILITY_REQ\n", DRV_NAME, tc));
		rtn = t_capability_req(tc, q, mp);
		break;
	default:
		printd(("%s: %p: -> ???\n", DRV_NAME, tc));
		rtn = t_other_req(tc, q, mp);
		break;
	}
	if (rtn < 0)
		tcap_set_t_state(tc, oldstate);
	return (rtn);
}
static int
tcap_w_proto_mgmt(struct tc *tc, queue_t *q, mblk_t *mp)
{
	t_uscalar_t oldstate = tcap_get_m_state(tc);
	int rtn;

	switch (*(t_uscalar_t *) mp->b_rptr) {
	default:
		rtn = TNOTSUPPORT;
		break;
	}
	if (rtn < 0) {
		tcap_set_m_state(tc, oldstate);
		return (rtn);
	}
	freemsg(mp);
	return (0);
}

/**
 * tcap_w_proto: - process M_PROTO or M_PCPROTO STREAMS message
 * @q: active queue (write queue)
 * @mp: the M_PROTO or M_PCPROTO message
 */
static inline fastcall __hot int
tcap_w_proto(queue_t *q, mblk_t *mp)
{
	struct tc *tc;
	int rtn;

	if (unlikely((tc = tc_acquire(q)) == NULL))
		goto edeadlk;

	switch (tc->i_style) {
	case TCAP_STYLE_TCI:
		rtn = tcap_w_proto_tci(tc, q, mp);
		break;
	case TCAP_STYLE_TRI:
		rtn = tcap_w_proto_tri(tc, q, mp);
		break;
	case TCAP_STYLE_TPI:
		rtn = tcap_w_proto_tpi(tc, q, mp);
		break;
	case TCAP_STYLE_MGMT:
		rtn = tcap_w_proto_mgmt(tc, q, mp);
		break;
	default:
		mi_strlog(q, 0, SL_ERROR | SL_TRACE, "%s: no interface style", __FUNCTION__);
		noenable(q);
		goto eagain;
	}

	tc_release(tc);

	return (rtn);

      eagain:
	return (-EAGAIN);
      edeadlk:
	return (-EDEADLK);
}

/**
 * tcap_w_sig: - process M_SIG or M_PCSIG STREAMS message
 * @q: active queue (write queue)
 * @mp: the M_SIG or M_PCSIG message
 */
static noinline fastcall int
tcap_w_sig(queue_t *q, mblk_t *mp)
{
	mi_strlog(q, 0, SL_ERROR | SL_TRACE, "invalid M_SIG message on upper write queue");
	freemsg(mp);
	return (0);
}

/**
 * tcap_w_data: - process M_DATA or M_HPDATA STREAMS message
 * @q: active queue (write queue)
 * @mp: the M_DATA or M_HPDATA message
 */
static noinline fastcall __unlikely int
tcap_w_data(queue_t *q, mblk_t *mp)
{
	struct tc *tc = TC_PRIV(q);

	switch (tc->i_style) {
#if 0
	case TCAP_STYLE_TRI:
		return tr_data(tc, q, mp);
	case TCAP_STYLE_TCI:
		return tc_data(tc, q, mp);
	case TCAP_STYLE_TPI:
		return t_data(tc, q, mp);
	case TCAP_STYLE_MGMT:
		return mgm_data(tc, q, mp);
#endif
	default:
		swerr();
		return (-EFAULT);
	}
}

/**
 * tcap_w_ioctl: - process M_IOCTL STREAMS message
 * @q: active queue (write queue)
 * @mp: the M_IOCTL message
 */
static noinline fastcall __unlikely int
tcap_w_ioctl(queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct tc *tc;
	int rtn;

	switch (_IOC_TYPE(ioc->ioc_cmd)) {
	case __SID:
		rtn = mgmt_ioctl(q, mp);
		break;
	case TCAP_IOC_MAGIC:
		if ((tc = tc_acquire(q)) == NULL)
			return (-EDEADLK);

		rtn = tcap_ioctl(tc, q, mp);

		tc_release(tc);
		break;
	default:
		rtn = EINVAL;
		break;
	}

	if (rtn > 0) {
		mi_copy_done(q, mp, rtn);
		rtn = 0;
	}
	return (rtn);
}

/**
 * tcap_w_iocdata: - process M_IOCDATA STREAMS message
 * @q: active queue (write queue)
 * @mp: the M_IOCDATA message
 */
static noinline fastcall __unlikely int
tcap_w_iocdata(queue_t *q, mblk_t *mp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	struct tc *tc;
	mblk_t *dp;
	int rtn;

	switch (_IOC_TYPE(cp->cp_cmd)) {
	case TCAP_IOC_MAGIC:
		if ((tc = tc_acquire(q)) == NULL)
			return (-EDEADLK);
		switch (mi_copy_state(q, mp, &dp)) {
		case -1:
			rtn = 0;
			break;
		case MI_COPY_CASE(MI_COPY_IN, 1):
			rtn = tcap_copyin1(tc, q, mp, dp);
			break;
		case MI_COPY_CASE(MI_COPY_IN, 2):
			rtn = tcap_copyin2(tc, q, mp, dp);
			break;
		case MI_COPY_CASE(MI_COPY_OUT, 1):
			rtn = tcap_copyout(tc, q, mp);
			break;
		default:
			rtn = EPROTO;
			break;
		}
		tc_release(tc);
		break;
	default:
		rtn = EINVAL;
		break;
	}
	if (rtn > 0) {
		mi_copy_done(q, mp, rtn);
		rtn = 0;
	}
	return (rtn);
}

/**
 * tcap_w_flush: - process M_FLUSH STREAMS message
 * @q: active queue (write queue)
 * @mp: the M_FLUSH message
 */
static noinline fastcall __unlikely int
tcap_w_flush(queue_t *q, mblk_t *mp)
{
	if (mp->b_rptr[0] & FLUSHW) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
		mp->b_rptr[0] &= ~FLUSHW;
	}
	if (mp->b_rptr[0] & FLUSHR) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(RD(q), mp->b_rptr[1], FLUSHDATA);
		else
			flushq(RD(q), FLUSHDATA);
		qreply(q, mp);
		return (0);
	}
	freemsg(mp);
	return (0);
}

/**
 * tcap_w_other: - process unhandled STREAMS message
 * @q: active queue (write queue)
 * @mp: the unhandled message
 */
static noinline fastcall __unlikely int
tcap_w_other(queue_t *q, mblk_t *mp)
{
	mi_strlog(q, 0, SL_ERROR | SL_TRACE, "invalid STREAMS message %d on upper write queue", (int) DB_TYPE(mp));
	freemsg(mp);
	return (0);
}

/**
 * tcap_w_prim_slow: - process STREAMS message
 * @q: active queue (write queue)
 * @mp: the STREAMS message
 */
static noinline fastcall __unlikely int
tcap_w_prim_slow(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_DATA:
	case M_HPDATA:
		return tcap_w_data(q, mp);
	case M_IOCTL:
		return tcap_w_ioctl(q, mp);
	case M_IOCDATA:
		return tcap_w_iocdata(q, mp);
	case M_FLUSH:
		return tcap_w_flush(q, mp);
	default:
		return tcap_w_other(q, mp);
	}
}

static inline __hot_write int
tcap_w_rse(queue_t *q, mblk_t *mp)
{
	freemsg(mp);
	return (0);
}

/**
 * tcap_w_prim: - process STREAMS message
 * @q: active queue (upper write queue)
 * @mp: the STREAMS message
 */
static inline __hot int
tcap_w_prim(queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(DB_TYPE(mp), M_PROTO)) {
	case M_PROTO:
	case M_PCPROTO:
		return tcap_w_proto(q, mp);
	case M_RSE:
	case M_PCRSE:
		return tcap_w_rse(q, mp);
	case M_SIG:
	case M_PCSIG:
		return tcap_w_sig(q, mp);
	}
	return tcap_w_prim_slow(q, mp);
}

static inline __hot_read int
tcap_r_rse(queue_t *q, mblk_t *mp)
{
	freemsg(mp);
	return (0);
}

/**
 * tcap_r_prim: - process STREAMS message
 * @q: active queue (upper read queue)
 * @mp: the STREAMS message
 *
 * Messages are passed to the read queue of an upper multiplex Stream to be passed along to the
 * module above.  The other messages that we place here are M_RSE or M_PCRSE messages.  M_RSE and
 * M_PCRSE messages are used internally to represent TCAP events.  An TCAP event can either be
 * transformed by encoding them as an SCCP message when passed to a lower multiplex Stream, or they
 * can be transformed into TCAP User primitives (TCI, TRI, TPI) when passed to an upper multiplex
 * Stream.  This later case is the case here.  M_RSE and M_PCRSE messages must be transformed into
 * appropriate TCAP User primitives before being passed upstream.  All other messages are passed
 * along directly as long as they are not subject to flow control.
 */
static inline __hot_read int
tcap_r_prim(queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(DB_TYPE(mp), M_RSE)) {
	case M_RSE:
	case M_PCRSE:
		return tcap_r_rse(q, mp);
	default:
		if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
			putnext(q, mp);
			return (0);
		}
		return (-EBUSY);
	}
}

static inline __hot_out int
sccp_w_rse(queue_t *q, mblk_t *mp)
{
	freemsg(mp);
	return (0);
}

/**
 * sccp_w_prim: - process STREAMS message
 * @q: active queue (lower write queue)
 * @mp: the STREAMS message
 *
 * Messages are passed to the write queue of a lower multiplex Stream to be passed along to the
 * driver below.  The other messages that we place here are M_RSE or M_PCRSE messages.  M_RSE and
 * M_PCRSE messages are used internally to represent TCAP events.  An TCAP event can either be
 * transformed by encoding them as an SCCP message when passed to a lower multiplex Stream, or they
 * can be transformed into TCAP User primitives (TCI, TRI, TPI) when passed to an upper multiplex
 * Stream.  The former case is the case here.  M_RSE and M_PCRSE messages must be transformed into
 * appropriate SCCP messages before being passed downstream.  All other messages are passed along
 * directly as long as they are not subject to flow control.
 */
static inline __hot_out int
sccp_w_prim(queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(DB_TYPE(mp), M_RSE)) {
	case M_RSE:
	case M_PCRSE:
		return sccp_w_rse(q, mp);
	default:
		if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
			putnext(q, mp);
			return (0);
		}
		return (-EBUSY);
	}
}

/**
 * sccp_r_proto: - process M_PROTO or M_PCPROTO STREAMS message
 * @q: active queue (read queue)
 * @mp: the M_PROTO or M_PCPROTO message
 */
static inline fastcall __hot int
sccp_r_proto(queue_t *q, mblk_t *mp)
{
	struct sc *sc = SC_PRIV(q);
	t_uscalar_t prim;
	int rtn, loglevel;

	if (mp->b_wptr < mp->b_rptr + sizeof(prim))
		goto emsgsize;

	prim = *(typeof(prim) *) mp->b_rptr;

	/* Acquire the SP lock for the lower Stream. */

	if ((sc = sc_acquire(q)) == NULL)
		goto edeadlk;

	sccp_save_i_state(sc);

	switch (prim) {
	case N_UNITDATA_IND:
	case N_UDERROR_IND:
	case N_EXDATA_IND:
	case N_DATA_IND:
		loglevel = STRLOGDA;
		break;
	default:
		loglevel = STRLOGRX;
		break;
	}

	mi_strlog(q, loglevel, SL_TRACE, "M_(PC)PROTO[%s] <-", sc_primname(prim));

	switch (*(t_uscalar_t *) mp->b_rptr) {
	case N_CONN_IND:
		rtn = n_conn_ind(sc, q, mp);
		break;
	case N_CONN_CON:
		rtn = n_conn_con(sc, q, mp);
		break;
	case N_DISCON_IND:
		rtn = n_discon_ind(sc, q, mp);
		break;
	case N_DATA_IND:
		rtn = n_data_ind(sc, q, mp);
		break;
	case N_EXDATA_IND:
		rtn = n_exdata_ind(sc, q, mp);
		break;
	case N_INFO_ACK:
		rtn = n_info_ack(sc, q, mp);
		break;
	case N_BIND_ACK:
		rtn = n_bind_ack(sc, q, mp);
		break;
	case N_ERROR_ACK:
		rtn = n_error_ack(sc, q, mp);
		break;
	case N_OK_ACK:
		rtn = n_ok_ack(sc, q, mp);
		break;
	case N_UNITDATA_IND:
		rtn = n_unitdata_ind(sc, q, mp);
		break;
	case N_UDERROR_IND:
		rtn = n_uderror_ind(sc, q, mp);
		break;
	case N_DATACK_IND:
		rtn = n_datack_ind(sc, q, mp);
		break;
	case N_RESET_IND:
		rtn = n_reset_ind(sc, q, mp);
		break;
	case N_RESET_CON:
		rtn = n_reset_con(sc, q, mp);
		break;
	case N_COORD_IND:
		rtn = n_coord_ind(sc, q, mp);
		break;
	case N_COORD_CON:
		rtn = n_coord_con(sc, q, mp);
		break;
	case N_STATE_IND:
		rtn = n_state_ind(sc, q, mp);
		break;
	case N_PCSTATE_IND:
		rtn = n_pcstate_ind(sc, q, mp);
		break;
	default:
		rtn = n_other_ind(sc, q, mp);
		break;
	}
	if (rtn < 0)
		sccp_restore_i_state(sc);

	sc_release(sc);

	return (rtn);

      edeadlk:
	return (-EDEADLK);
      emsgsize:
	return (-EMSGSIZE);
}

/**
 * sccp_r_rse: - process M_RSE or M_PCRSE STREAMS message
 * @q: active queue (read queue)
 * @mp: the M_RSE or M_PCRSE message
 */
static inline fastcall __unlikely int
sccp_r_rse(queue_t *q, mblk_t *mp)
{
	mi_strlog(q, STRLOGRX, SL_TRACE, "M_(PC)RSE <-");
	freemsg(mp);
	return (0);
}

/**
 * sccp_r_sig: - process M_SIG or M_PCSIG STREAMS message
 * @q: active queue (read queue)
 * @mp: the M_SIG or M_PCSIG message
 */
static inline fastcall __unlikely int
sccp_r_sig(queue_t *q, mblk_t *mp)
{
	mi_strlog(q, STRLOGRX, SL_TRACE, "M_(PC)SIG <-");
	freemsg(mp);
	return (0);
}

/**
 * sccp_r_data: - process M_DATA STREAMS message
 * @q: active queue (read queue)
 * @mp: the M_DATA message
 */
static noinline fastcall __unlikely int
sccp_r_data(queue_t *q, mblk_t *mp)
{
	struct sc *sc = SC_PRIV(q);

	mi_strlog(q, STRLOGRX, SL_TRACE, "M_(HP)DATA <-");
	/* FIXME: n_data_ind() cannot handle M_DATA blocks! */
	return n_data_ind(sc, q, mp);
}

/**
 * sccp_r_copy: - process M_COPYIN or M_COPYOUT STREAMS message
 * @q: active queue (read queue)
 * @mp: the M_COPYIN or M_COPYOUT message
 */
static noinline fastcall __unlikely int
sccp_r_copy(queue_t *q, mblk_t *mp)
{
	mi_strlog(q, STRLOGRX, SL_TRACE, "M_COPYIN/OUT <-");
	freemsg(mp);
	return (0);
}

/**
 * sccp_r_iocack: - process M_IOCACK or M_IOCNAK STREAMS message
 * @q: active queue (read queue)
 * @mp: the M_IOCACK or M_IOCNAK message
 */
static noinline fastcall __unlikely int
sccp_r_iocack(queue_t *q, mblk_t *mp)
{
	mi_strlog(q, STRLOGRX, SL_TRACE, "M_IOCACK/NAK <-");
	freemsg(mp);
	return (0);
}

/**
 * sccp_r_flush: - process M_FLUSH STREAMS message
 * @q: active queue (read queue)
 * @mp: the M_FLUSH message
 *
 * Canonical lower multiplex Stream flushing procedure.  Also, handling looping in the same fashion
 * as the Stream head (because a lower multiplex Stream replaces the Stream head).
 */
static noinline fastcall __unlikely int
sccp_r_flush(queue_t *q, mblk_t *mp)
{
	mi_strlog(q, STRLOGRX, SL_TRACE, "M_FLUSH <-");
	if (mp->b_rptr[0] & FLUSHR) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
		mp->b_rptr[0] &= ~FLUSHW;
	}
	if ((mp->b_flag & MSGNOLOOP))
		goto discard;
	if (mp->b_rptr[0] & FLUSHW) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(WR(q), mp->b_rptr[1], FLUSHDATA);
		else
			flushq(WR(q), FLUSHDATA);
		mp->b_flag |= MSGNOLOOP;
		qreply(q, mp);
		return (0);
	}
      discard:
	freemsg(mp);
	return (0);
}

/**
 * sccp_r_error: - process M_ERROR STREAMS message
 * @q: active queue (read queue)
 * @mp: the M_ERROR message
 */
static noinline fastcall __unlikely int
sccp_r_error(queue_t *q, mblk_t *mp)
{
	struct sc *sc = SC_PRIV(q);

	mi_strlog(q, STRLOGRX, SL_TRACE, "M_ERROR <-");
	sccp_set_i_state(sc, NS_NOSTATES);
	/* TODO: need a notification of error on a lower stream. */
	freemsg(mp);
	return (0);
}

/**
 * sccp_r_hangup: - process M_HANGUP STREAMS message
 * @q: active queue (read queue)
 * @mp: the M_HANGUP message
 */
static noinline fastcall __unlikely int
sccp_r_hangup(queue_t *q, mblk_t *mp)
{
	struct sc *sc = SC_PRIV(q);

	mi_strlog(q, STRLOGRX, SL_TRACE, "M_HANGUP <-");
	sccp_set_i_state(sc, NS_NOSTATES);
	/* TODO: need a notification of hangup on a lower stream. */
	freemsg(mp);
	return (0);
}

/**
 * sccp_r_unhangup: - process M_UNHANGUP STREAMS message
 * @q: active queue (read queue)
 * @mp: the M_UNHANGUP message
 */
static noinline fastcall __unlikely int
sccp_r_unhangup(queue_t *q, mblk_t *mp)
{
	struct sc *sc = SC_PRIV(q);

	mi_strlog(q, STRLOGRX, SL_TRACE, "M_UNHANGUP <-");
	return n_info_req(sc, q, mp);
}

/**
 * sccp_r_setopts: - process M_SETOPTS or M_PCSETOPTS STREAMS message
 * @q: active queue (read queue)
 * @mp: the M_SETOPTS or M_PCSETOPTS message
 */
static noinline fastcall __unlikely int
sccp_r_setopts(queue_t *q, mblk_t *mp)
{
	struct sc *sc = SC_PRIV(q);

	mi_strlog(q, STRLOGRX, SL_TRACE, "%s <-", msgname(DB_TYPE(mp)));
	return n_info_req(sc, q, mp);
}

/**
 * sccp_r_ioctl: - process M_IOCTL STREAMS message
 * @q: active queue (read queue)
 * @mp: the M_IOCTL message
 */
static noinline fastcall __unlikely int
sccp_r_ioctl(queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;

	mi_strlog(q, STRLOGRX, SL_TRACE, "M_IOCTL <-");
	ioc->ioc_error = EINVAL;
	ioc->ioc_rval = -1;
	DB_TYPE(mp) = M_IOCNAK;
	mi_strlog(q, STRLOGTX, SL_TRACE, "M_IOCNAK ->");
	qreply(q, mp);
	return (0);
}

/**
 * sccp_r_read: - process M_READ STREAMS message
 * @q: active queue (read queue)
 * @mp: the M_READ message
 */
static noinline fastcall __unlikely int
sccp_r_read(queue_t *q, mblk_t *mp)
{
	mblk_t *bp;

	if ((bp = mi_allocb(q, 0, BPRI_MED))) {
		freemsg(mp);
		mi_strlog(q, STRLOGRX, SL_TRACE, "M_READ <-");
		mi_strlog(q, STRLOGTX, SL_TRACE, "M_DATA ->");
		qreply(q, bp);
		return (0);
	}
	return (-ENOBUFS);
}

#ifdef M_LETSPLAY
/**
 * sccp_r_letsplay: - process M_LETSPLAY STREAMS message
 * @q: active queue (read queue)
 * @mp: the M_LETSPLAY message
 */
static noinline fastcall __unlikely int
sccp_r_letsplay(queue_t *q, mblk_t *mp)
{
	mi_strlog(q, STRLOGRX, SL_TRACE, "M_LETSPLAY <-");
	DB_TYPE(mp) = M_DONTPLAY;
	mi_strlog(q, STRLOGTX, SL_TRACE, "M_DONTPLAY ->");
	qreply(q, mp);
	return (0);
}
#endif				/* M_LETSPLAY */

#ifdef M_MI
/**
 * sccp_r_mi: - process M_MI STREAMS message
 * @q: active queue (read queue)
 * @mp: the M_MI message
 */
static noinline fastcall __unlikely int
sccp_r_mi(queue_t *q, mblk_t *mp)
{
	mi_strlog(q, STRLOGRX, SL_TRACE, "M_MI <-");
	freemsg(mp);
	return (0);
}
#endif				/* M_MI */

/**
 * sccp_r_other: - process unhandled STREAMS message
 * @q: active queue (read queue)
 * @mp: the unhandled message
 *
 * On a lower multiplex Stream, unhandled messages, for the most part, should be handled in a
 * fashion consistent with the Stream head.
 */
static noinline fastcall __unlikely int
sccp_r_other(queue_t *q, mblk_t *mp)
{
	mi_strlog(q, STRLOGRX, SL_TRACE, "%s <-", msgname(DB_TYPE(mp)));
	freemsg(mp);
	return (0);
}

static noinline fastcall __unlikely int
sccp_r_prim_slow(queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_DATA:
	case M_HPDATA:
		return sccp_r_data(q, mp);
	case M_COPYIN:
	case M_COPYOUT:
		return sccp_r_copy(q, mp);
	case M_IOCACK:
	case M_IOCNAK:
		return sccp_r_iocack(q, mp);
	case M_FLUSH:
		return sccp_r_flush(q, mp);
	case M_ERROR:
		return sccp_r_error(q, mp);
	case M_HANGUP:
		return sccp_r_hangup(q, mp);
	case M_UNHANGUP:
		return sccp_r_unhangup(q, mp);
	case M_SETOPTS:
	case M_PCSETOPTS:
		return sccp_r_setopts(q, mp);
	case M_IOCTL:
		return sccp_r_ioctl(q, mp);
	case M_READ:
		return sccp_r_read(q, mp);
#ifdef M_LETSPLAY
	case M_LETSPLAY:
		return sccp_r_letsplay(q, mp);
#endif				/* M_LETSPLAY */
#ifdef M_MI
	case M_MI:
		return sccp_r_mi(q, mp);
#endif				/* M_MI */
	default:
		return sccp_r_other(q, mp);
	}
}

/**
 * sccp_r_prim: - process STREAMS message
 * @q: active queue (read queue)
 * @mp: the STREAMS message
 *
 * This function must return non-zero when the message is to be placed onto (put) or back onto
 * (service) the queue and must return zero (0) once it has disposed of the message.
 */
static inline fastcall int
sccp_r_prim(queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(DB_TYPE(mp), M_PROTO)) {
	case M_PROTO:
	case M_PCPROTO:
		return sccp_r_proto(q, mp);
	case M_RSE:
	case M_PCRSE:
		return sccp_r_rse(q, mp);
	case M_SIG:
	case M_PCSIG:
		return sccp_r_sig(q, mp);
	}
	return sccp_r_prim_slow(q, mp);
}

/*
 *  =========================================================================
 *
 *  PUT and SRV
 *
 *  =========================================================================
 */

static streamscall __hot_write int
tcap_wput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || tcap_w_prim(q, mp))
		putq(q, mp);
	return (0);
}

static streamscall __hot_out int
tcap_wsrv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		if (tcap_w_prim(q, mp)) {
			putbq(q, mp);
			break;
		}
	}
	return (0);
}

static streamscall __unlikely int
tcap_rput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || tcap_r_prim(q, mp))
		putq(q, mp);
	return (0);
}

static streamscall __hot_read int
tcap_rsrv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		if (tcap_r_prim(q, mp)) {
			putbq(q, mp);
			break;
		}
	}
	return (0);
}
static streamscall __hot_out int
sccp_wsrv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		if (sccp_w_prim(q, mp)) {
			putbq(q, mp);
			break;
		}
	}
	return (0);
}

static streamscall __unlikely int
sccp_wput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || sccp_w_prim(q, mp))
		putq(q, mp);
	return (0);
}

static streamscall __hot_read int
sccp_rsrv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		if (sccp_r_prim(q, mp)) {
			putbq(q, mp);
			break;
		}
	}
	return (0);
}

static streamscall __hot_in int
sccp_rput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || sccp_r_prim(q, mp))
		putq(q, mp);
	return (0);
}

/*
 *  =========================================================================
 *
 *  OPEN and CLOSE
 *
 *  =========================================================================
 */
static int tcap_majors[CMAJORS] = { CMAJOR_0, };

#ifndef NUM_TE
#define NUM_TE 32
#endif

/**
 * tcap_qopen: - STREAMS driver open routine
 * @q: newly created queue pair
 * @devp: pointer to device number associated with Stream
 * @oflags: flags to the open(2s) call
 * @sflag: STREAMS flag
 * @crp: pointer to the credentials of the opening process
 *
 * When a Stream is opened on the driver it corresponds to a AC associated with a given TCAP Entity
 * (TE).  The TE is determined from the minor device opened.  All minor devices corresponding to TEs
 * are clone or auto-clone devices.  There may be several SCCP lower Streams for each TE (one for
 * each SP associated with the TE).  If a TE structure has not been allocated for the corresponding
 * minor device number, we allocate one.  When an SCCP Stream is I_LINK'ed under the driver, it is
 * associated with the TE structure.  A TC and AC structure is allocated and associated with each
 * upper Stream.
 *
 * When a Stream is opened on the driver it corresponds to a TCAP Stream associated with a given
 * interface style.  The interface style is also determined from the minor device opened.  All minor
 * devices corresponding to interface sytles are clone or auto-clone devices.
 *
 * This driver cannot be pushed as a module.
 *
 * (cminor == 0) && (sflag == DRVOPEN)
 *	When minor device number 0 is opened with DRVOPEN (non-clone), a control Stream is opened.
 *	If a control Stream has already been opened, the open is refused.  The sflag is changed from
 *	DRVOPEN to CLONEOPEN and a new minor device number is assigned.  This uses the autocloning
 *	features of Linux Fast-STREAMS.  This corresponds to the /dev/streams/tcap/mgmt minor device
 *	node.
 *
 * (cminor == 0) && (sflag == CLONEOPEN)
 *	This is a normal clone open using the clone(4) driver.  A disassociated user Stream is
 *	opened.  A new unique minor device number is assigned.  This corresponds to the
 *	/dev/streams/clone/tcap clone device node.  The interface provided is the Transaction
 *	Component  Interface (TCI).
 *
 * (cminor == 0, NUM_TE, NUM_TE*2, NUM_TE*3) && (sflag == DRVOPEN)
 *	This is a normal non-clone open.  Where the minor device number is NUM_TE, NUM_TE*2 or
 *	NUM_TE*3, a disassociated user Stream is opened.  A new unique minor device number is
 *	assigned.  This uses the autocloning features of Linux Fast-STREAMS.  The interface provided
 *	depends upon the device number opened.  NUM_TE provides the TCI interface.  0, provides the
 *	management interface.  NUM_TE*2 provides the TRI Interface.  NUM_TE*3 provides the TPI
 *	Interface.  These correspond to the /dev/streams/tcap/mgmt, /dev/streams/tcap/tc,
 *	/dev/streams/tcap/tr, and /dev/streams/tcap/tp minor device nodes.
 *
 * ((       0 < cminor && cminor <   NUM_TE) ||
 *  (  NUM_TE < cminor && cminor < 2*NUM_TE) ||
 *  (2*NUM_TE < cminor && cminor < 3*NUM_TE)) && (sflag == DRVOPEN))
 *	This is a normal non-clone open.  Where the minor device number is between 0 and NUM_TE,
 *	NUM_TE and 2*NUM_TE, or 2*NUM_TE and 3*NUM_S, an associated user Stream is opened.  If
 *	there is no subsystem structure to associate, one is created with default values.  A new
 *	minor device number above 3*NUM_TE is assigned.  This uses the autocloning features of Linux
 *	Fast-STREAMS.  This corresponds to the /dev/streams/tcap/NNNN minor device node where NNNN
 *	is the minor device number.
 */
static int
tcap_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	psw_t flags;
	major_t cmajor = getmajor(*devp);
	minor_t cminor = getminor(*devp);
	struct tc *tc;
	struct te *te;
	int err = 0;

	if (q->q_ptr != NULL)
		return (0);	/* already open */
	if (sflag == MODOPEN || q->q_next)
		return (ENXIO);
	if (cminor > 3 * NUM_TE)
		return (ENXIO);
	if (!(tc = tcap_open_alloc(q, devp, crp, cminor)))
		return (ENOMEM);

	write_lock_irqsave(&tcap_mux_lock, flags);

	if (cminor == 0) {
		/* When a zero minor device was opened, the stream is either a clone open or an
		   attempt to open the master control Stream.  The difference is whether the sflag
		   is DRVOPEN or CLONEOPEN. */
		if (sflag == DRVOPEN) {
			/* When the master control Stream is opened, another master control Stream
			   must not yet exist.  If this is the only mater control Stream then it is
			   created. */
			if (lm_ctrl != NULL) {
				write_unlock_irqrestore(&tcap_mux_lock, flags);
				tcap_close_free(tc);
				return (ENXIO);
			}
		}
		*devp = makedevice(cmajor, (3 * NUM_TE) + 1);
		/* start assigning minors at 3*NUM_TE + 1 */
		if ((err = mi_open_link(&tcap_opens, (caddr_t) tc, devp, oflags, CLONEOPEN, crp))) {
			write_unlock_irqrestore(&tcap_mux_lock, flags);
			tcap_close_free(tc);
			return (err);
		}
		if (sflag == DRVOPEN)
			lm_ctrl = tc;
		/* Both master control Streams and clone user Streams are disassociated with any
		   specific TE.  Master control Streams are never assocaited with a specific TE.
		   User Streams are associated with an TE using the SCCP adress and Application
		   Context to the bind primitive, or when an SCCP Stream is temporarily linked
		   under the drivers using the I_LINK input-output control. */
	} else {
		DECLARE_WAITQUEUE(wait, current);

		/* When a non-zero minor device number was opened, the Stream is automaticlaly
		   associated with the AC to thwich the minor device number corresponds.  It cannot
		   be disassociated except when it is closed. */
		if (!(te = te_lookup(tc, cminor))) {
			write_unlock_irqrestore(&tcap_mux_lock, flags);
			tcap_close_free(tc);
			return (ENXIO);
		}
		/* Locking: need to wait until a lock on the AC structure can be acquired, or a
		   signal is received, or the AC structure is deallocated.  If the lock can be
		   acquired, associate the User Stream with the AC structure; in all other cases,
		   return an error.  Note that it is a likely event that the lock can be acquired
		   without waiting. */
		err = 0;
		add_wait_queue(&tcap_waitq, &wait);
		spin_lock(&te->sq.lock);
		for (;;) {
			set_current_state(TASK_INTERRUPTIBLE);
			if (signal_pending(current)) {
				err = EINTR;
				spin_unlock(&te->sq.lock);
				break;
			}
			if (te->sq.users != 0) {
				spin_unlock(&te->sq.lock);
				write_unlock_irqrestore(&tcap_mux_lock, flags);
				schedule();
				write_lock_irqsave(&tcap_mux_lock, flags);
				if (!(te = te_lookup(tc, cminor))) {
					err = ENXIO;
					break;
				}
				spin_lock(&te->sq.lock);
				continue;
			}
			*devp = makedevice(cmajor, (3 * NUM_TE) + 1);
			/* start assigning minors at 3*NUM_TE + 1 */
			err = mi_open_link(&tcap_opens, (caddr_t) tc, devp, oflags, CLONEOPEN, crp);
			if (err == 0)
				tcap_attach(tc, te);
			spin_unlock(&te->sq.lock);
			break;
		}
		set_current_state(TASK_RUNNING);
		remove_wait_queue(&tcap_waitq, &wait);
		if (err) {
			write_unlock_irqrestore(&tcap_mux_lock, flags);
			tcap_close_free(tc);
			return (err);
		}
	}

	/* just a few fixups after device number assigned */
	tc->dev = *devp;

	write_unlock_irqrestore(&tcap_mux_lock, flags);

	mi_attach(q, (caddr_t) tc);
	qprocson(q);
	return (0);
}

/**
 * tcap_qclose: - STREAMS driver close routine
 * @q: queue pair
 * @oflags: flags to the open(2s) call
 * @crp: pointer to the credentials of the closing process
 */
static int
tcap_qclose(queue_t *q, int oflags, cred_t *crp)
{
	struct tc *tc = TC_PRIV(q);
	psw_t flags;
	struct te *te;

	qprocsoff(q);
	mi_detach(q, (caddr_t) tc);
	write_lock_irqsave(&tcap_mux_lock, flags);
	if ((te = tc->te.te)) {
		DECLARE_WAITQUEUE(wait, current);
		/* Locking: need to wait until a TE lock can be acquired, or the TE structure is
		   deallocated.  If a lock can be acquired, the closing Stream is disassociated
		   with the TE; otherwise, if the TE structure is deallocated, there is no further
		   need to disassociate.  Note that it is a likely event that the lock can be
		   acquired without waiting. */
		add_wait_queue(&tcap_waitq, &wait);
		spin_lock(&te->sq.lock);
		for (;;) {
			set_current_state(TASK_UNINTERRUPTIBLE);
			if (te->sq.users == 0) {
				tcap_detach(tc);
				spin_unlock(&te->sq.lock);
				break;
			}
			spin_unlock(&te->sq.lock);
			write_unlock_irqrestore(&tcap_mux_lock, flags);
			schedule();
			write_lock_irqsave(&tcap_mux_lock, flags);
			if (!(te = tc->te.te))
				break;
			spin_lock(&te->sq.lock);
		}
		set_current_state(TASK_RUNNING);
		remove_wait_queue(&tcap_waitq, &wait);
	}
	mi_close_unlink(&tcap_opens, (caddr_t) tc);
	write_unlock_irqrestore(&tcap_mux_lock, flags);
	tcap_close_free(tc);
	return (0);
}

/*
 *  =========================================================================
 *
 *  Allocation and deallocation
 *
 *  =========================================================================
 */

/*
 *  TCAP Structure
 *  -----------------------------------
 */
static struct tc *
tcap_alloc_priv(queue_t *q, struct tc **tcapp, dev_t *devp, cred_t *credp, minor_t minor)
{
	(void) tcap_alloc_priv;
	return (NULL);
}
static inline struct tc *
tc_get(struct tc *tc)
{
	return (tc);
}
static struct tc *
tc_lookup(uint id)
{
	return (NULL);
}
static inline uint
tc_get_id(uint id)
{
	return (0);
}
static void
tcap_free_priv(struct tc *tc)
{
	(void) tcap_free_priv;
	return;
}
static inline void
tc_put(struct tc *tc)
{
	return;
}

static struct tc *__unlikely
tcap_alloc_object_tc(int *errp)
{
	struct tc *tc;

#if 0
	tc = (typeof(tc)) mi_open_alloc_cache(tcap_priv_cachep, GFP_KERNEL);
#else
	tc = (typeof(tc)) mi_open_alloc_sleep(sizeof(*tc));
#endif
	if (tc) {
		bzero(tc, sizeof(*tc));
	} else if (errp)
		*errp = ENOMEM;
	return (tc);
}
static void __unlikely
tcap_init_object_tc(struct tc *tc, minor_t unit)
{
	return;
}
static void __unlikely
tcap_term_object_tc(struct tc *tc)
{
	/* FIXME: need to abort all transactions, dialogues and invocations. */
	return;			/* handled mostly by mi_ functions */
}
static void __unlikely
tcap_free_object_tc(struct tc *tc)
{
#if 0
	mi_close_free_cache(tcap_priv_cachep, (caddr_t) tc);
#else
	mi_close_free((caddr_t) tc);
#endif
}

static struct tc *
tcap_open_alloc(queue_t *q, dev_t *devp, cred_t *credp, minor_t unit)
{
	struct tc *tc;
	int err = 0;

	if ((tc = tcap_alloc_object_tc(&err))) {
		tcap_init_object_tc(tc, unit);
		tc->iq = WR(q);
		tc->oq = RD(q);
		tc->cred = *credp;
		tc->dev = *devp;
		tc->id = unit;
	}
	return (tc);
}
static void
tcap_close_free(struct tc *tc)
{
	tcap_term_object_tc(tc);
	tcap_free_object_tc(tc);
}

/*
 *  Invoke Structure
 *  -----------------------------------
 */
static inline struct iv *
tcap_alloc_iv(uint id)
{
	return (NULL);
}
static inline struct iv *
iv_get(struct iv *iv)
{
	return (iv);
}
static struct iv *
iv_lookup(struct dg *dg, uint id)
{
	return (NULL);
}
static inline uint
iv_get_id(uint id)
{
	return (0);
}
static inline void
tcap_free_iv(struct iv *iv)
{
	return;
}
static inline void
iv_put(struct iv *iv)
{
	return;
}

/*
 *  Operation Structure
 *  -----------------------------------
 */
static inline struct op *
tcap_alloc_op(uint id)
{
	return (NULL);
}
static inline struct op *
op_get(struct op *op)
{
	return (op);
}
static struct op *
op_lookup(struct ac *ac, uint id)
{
	return (NULL);
}
static inline uint
op_get_id(uint id)
{
	return (0);
}
static inline void
tcap_free_op(struct op *op)
{
	return;
}
static inline void
op_put(struct op *op)
{
	return;
}

/*
 *  Dialogue Structure
 *  -----------------------------------
 */
static inline struct dg *
tcap_alloc_dg(uint id)
{
	return (NULL);
}
static inline struct dg *
dg_get(struct dg *dg)
{
	return (dg);
}
static struct dg *
dg_lookup(struct tc *tc, uint id)
{
	return (NULL);
}
static inline uint
dg_get_id(uint id)
{
	return (0);
}
static inline void
tcap_free_dg(struct dg *dg)
{
	return;
}
static inline void
dg_put(struct dg *dg)
{
	return;
}

/*
 *  Application Context Structure
 *  -----------------------------------
 */
static inline struct ac *
tcap_alloc_ac(uint id)
{
	return (NULL);
}
static inline struct ac *
ac_get(struct ac *ac)
{
	return (ac);
}
static struct ac *
ac_lookup(struct tc *tc, uint id)
{
	return (NULL);
}
static inline uint
ac_get_id(uint id)
{
	return (0);
}
static inline void
tcap_free_ac(struct ac *ac)
{
	return;
}
static inline void
ac_put(struct ac *ac)
{
	return;
}

/*
 *  Transaction Structure
 *  -----------------------------------
 */
static struct tr *
tcap_alloc_tr(struct tc *tc, uint id, uint id2)
{
	return (NULL);
}
static inline struct tr *
tr_get(struct tr *tr)
{
	return (tr);
}
static struct tr *
tr_lookup_cid(struct tc *tc, uint id)
{
	return (NULL);
}
static struct tr *
tr_lookup_tid(struct tc *tc, uint id)
{
	return (NULL);
}
static inline uint
tr_get_id(uint id)
{
	return (0);
}
static inline void
tcap_free_tr(struct tr *tr)
{
	return;
}
static inline void
tr_put(struct tr *tr)
{
	return;
}

/*
 *  TCAP Entity
 *  -----------------------------------
 */
static struct te *
tcap_alloc_te(struct tc *tc, uint id)
{
	return (NULL);
}
static inline struct te *
te_get(struct te *te)
{
	return (te);
}
static struct te *
te_lookup(struct tc *tc, uint id)
{
	return (NULL);
}
static inline uint
te_get_id(uint id)
{
	return (0);
}
static void
tcap_free_te(struct te *te)
{
	return;
}
static inline void
te_put(struct te *te)
{
	return;
}

/*
 *  SCCP SAP Structure
 *  -----------------------------------
 */
static inline struct sp *
tcap_alloc_sp(uint id)
{
	return (NULL);
}
static inline struct sp *
sp_get(struct sp *sp)
{
	return (sp);
}
static struct sp *
sp_lookup(struct tc *tc, uint id)
{
	return (NULL);
}
static inline uint
sp_get_id(uint id)
{
	return (0);
}
static inline void
tcap_free_sp(struct sp *sp)
{
	return;
}
static inline void
sp_put(struct sp *sp)
{
	return;
}

/*
 *  SCCP Structure
 *  -----------------------------------
 */
static struct sc *
sccp_alloc_link(queue_t *q, int index, cred_t *crp, struct te *te)
{
	return (NULL);
}
static inline struct sc *
sc_get(struct sc *sc)
{
	return (sc);
}
static struct sc *
sc_lookup(uint id)
{
	return (NULL);
}
static inline uint
sc_get_id(uint id)
{
	return (0);
}
static void
sccp_free_unlink(struct sc *sc)
{
	return;
}
static inline void
sc_put(struct sc *sc)
{
	return;
}

/*
 *  =========================================================================
 *
 *  STREAMS initialization
 *
 *  =========================================================================
 */

static struct module_info tcap_minfo = {
	.mi_idnum = DRV_ID,		/* Module ID number */
	.mi_idname = DRV_NAME,		/* Module ID name */
	.mi_minpsz = 1,			/* Min packet size accepted */
	.mi_maxpsz = 272 + 1,		/* Max packet size accepted */
	.mi_hiwat = 1 << 15,		/* Hi water mark */
	.mi_lowat = 1 << 10,		/* Lo water mark */
};

static struct module_stat tcap_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat tcap_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat sccp_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat sccp_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

static struct qinit tcap_rinit = {
	.qi_putp = tcap_rput,		/* Read put (message from below) */
	.qi_srvp = tcap_rsrv,		/* Read queue service */
	.qi_qopen = tcap_qopen,		/* Each open */
	.qi_qclose = tcap_qclose,	/* Last close */
	.qi_minfo = &tcap_minfo,	/* Information */
	.qi_mstat = &tcap_rstat,	/* Statistics */
};

static struct qinit tcap_winit = {
	.qi_putp = tcap_wput,		/* Write put (message from above) */
	.qi_srvp = tcap_wsrv,		/* Write queue service */
	.qi_minfo = &tcap_minfo,	/* Information */
	.qi_mstat = &tcap_wstat,	/* Statistics */
};

static struct qinit sccp_rinit = {
	.qi_putp = sccp_rput,		/* Read put (message from below) */
	.qi_srvp = sccp_rsrv,		/* Read queue service */
	.qi_minfo = &tcap_minfo,	/* Information */
	.qi_mstat = &sccp_rstat,	/* Statistics */
};

static struct qinit sccp_winit = {
	.qi_putp = sccp_wput,		/* Write put (message from above) */
	.qi_srvp = sccp_wsrv,		/* Write queue service */
	.qi_minfo = &tcap_minfo,	/* Information */
	.qi_mstat = &sccp_wstat,	/* Statistics */
};

static struct streamtab tcinfo = {
	.st_rdinit = &tcap_rinit,	/* Upper read queue */
	.st_wrinit = &tcap_winit,	/* Upper write queue */
	.st_muxrinit = &sccp_rinit,	/* Lower read queue */
	.st_muxwinit = &sccp_winit,	/* Lower write queue */
};

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

unsigned short modid = DRV_ID;

#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0444);
#endif
MODULE_PARM_DESC(modid, "Module ID for the TCAP driver. (0 for allocation.)");

major_t major = CMAJOR_0;

#ifndef module_param
MODULE_PARM(major, "h");
#else
module_param(major, uint, 0444);
#endif
MODULE_PARM_DESC(major, "Device number for the TCAP driver. (0 for allocation.)");

/*
 *  Linux Fast-STREAMS Registration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#ifdef LFS

static struct cdevsw tcap_cdev = {
	.d_name = DRV_NAME,
	.d_str = &tcinfo,
	.d_flag = D_MP,
	.d_fop = NULL,
	.d_mode = S_IFCHR,
	.d_kmod = THIS_MODULE,
};

static int
tcap_register_strdev(major_t major)
{
	int err;

	if ((err = register_strdev(&tcap_cdev, major)) < 0)
		return (err);
	return (0);
}

static int
tcap_unregister_strdev(major_t major)
{
	int err;

	if ((err = unregister_strdev(&tcap_cdev, major)) < 0)
		return (err);
	return (0);
}

#endif				/* LFS */

/*
 *  Linux STREAMS Registration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#ifdef LIS

static int
tcap_register_strdev(major_t major)
{
	int err;

	if ((err = lis_register_strdev(major, &tcinfo, UNITS, DRV_NAME)) < 0)
		return (err);
	if (major == 0)
		major = err;
	if ((err = lis_register_driver_qlock_option(major, LIS_QLOCK_NONE)) < 0) {
		lis_unregister_strdev(major);
		return (err);
	}
	return (0);
}

static int
tcap_unregister_strdev(major_t major)
{
	int err;

	if ((err = lis_unregister_strdev(major)) < 0)
		return (err);
	return (0);
}

#endif				/* LIS */

static void __exit
tcapterminate(void)
{
	int err, mindex;

	for (mindex = CMAJORS - 1; mindex >= 0; mindex--) {
		if (tcap_majors[mindex]) {
			if ((err = tcap_unregister_strdev(tcap_majors[mindex])))
				cmn_err(CE_PANIC, "%s: cannot unregister major %d", DRV_NAME,
					tcap_majors[mindex]);
			if (mindex)
				tcap_majors[mindex] = 0;
		}
	}
#if 0
	if ((err = tcap_term_caches()))
		cmn_err(CE_WARN, "%s: could not terminate caches", DRV_NAME);
#endif
	return;
}

static int __init
tcapinit(void)
{
	int err, mindex = 0;

	cmn_err(CE_NOTE, DRV_BANNER);	/* console splash */
#if 0
	if ((err = tcap_init_caches())) {
		cmn_err(CE_WARN, "%s: could not init caches, err = %d", DRV_NAME, err);
		tcapterminate();
		return (err);
	}
#endif
	for (mindex = 0; mindex < CMAJORS; mindex++) {
		if ((err = tcap_register_strdev(tcap_majors[mindex])) < 0) {
			if (mindex) {
				cmn_err(CE_WARN, "%s: could not register major %d", DRV_NAME,
					tcap_majors[mindex]);
				continue;
			} else {
				cmn_err(CE_WARN, "%s: could not register driver, err = %d",
					DRV_NAME, err);
				tcapterminate();
				return (err);
			}
		}
		if (tcap_majors[mindex] == 0)
			tcap_majors[mindex] = err;
#if 0
		LIS_DEVFLAGS(tcap_majors[mindex]) |= LIS_MODFLG_CLONE;
#endif
		if (major == 0)
			major = tcap_majors[0];
	}
	return (0);
}

/*
 *  Linux Kernel Module Initialization
 *  -------------------------------------------------------------------------
 */
module_init(tcapinit);
module_exit(tcapterminate);

#endif				/* LINUX */
