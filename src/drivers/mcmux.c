/*****************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2009  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

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

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

#ident "@(#) $RCSfile$ $Name$($Revision$) $Date$"

static char const ident[] = "$RCSfile$ $Name$($Revision$) $Date$";

/*
 * This is MCMUX, a GSM MAP Common Services (MC) driver implementing the MAPI
 * interfaces as described in 3GPP TS 29.002 as a driver linked over TC
 * streams implementing the TCI interface.  This driver implemeents all of the
 * MAP Common Services interface described in 3GPP TS 29.002.
 */

#define _DEBUG 1

#define _SVR4_SOURCE 1
#define _MPS_SOURCE 1

#include <sys/os7/compat.h>
#include <sys/strsun.h>

#include <sys/sad.h>

#define MCMUX_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define MCMUX_EXTRA	"Part of the OpenSS7 Stack for Linux Fast-STREAMS"
#define MCMUX_COPYRIGHT	"Copyright (c) 2008-2009  Monavacon Limited.  All Rights Reserved."
#define MCMUX_REVISION	"Monavacon $RCSfile: mcmux.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2009-03-05 15:51:27 $"
#define MCMUX_DEVICE	"SVR 4.2 STREAMS 3GPP TS 29.002 MAP Common Services (MC) Driver"
#define MCMUX_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define MCMUX_LICENSE	"GPL"
#define MCMUX_BANNER	MCMUX_DESCRIP	"\n" \
			MCMUX_EXTRA	"\n" \
			MCMUX_REVISION	"\n" \
			MCMUX_COPYRIGHT	"\n" \
			MCMUX_DEVICE	"\n" \
			MCMUX_CONTACT
#define MCMUX_SPLASH	MCMUX_DESCRIPT	"\n" \
			MCMUX_REVISION

#ifdef LINUX
MODULE_AUTHOR(MCMUX_CONTACT);
MODULE_DESCRIPTION(MCMUX_DESCRIP);
MODULE_SUPPORTED_DEVICE(MCMUX_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(MCMUX_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-mcmux");
#endif				/* MODULE_ALIAS */
#ifdef MODULE_VERSION
MODULE_VERSION(__stringify(PACKAGE_RPMEPOCH) ":" PACKAGE_VERSION "." PACKAGE_RELEASE
	       PACKAGE_PATCHLEVEL "-" PACKAGE_RPMRELEASE PACKAGE_RPMEXTRA2);
#endif				/* MODULE_VERSION */
#endif				/* LINUX */

#define MCMUX_DRV_ID	CONFIG_STREAMS_MCMUX_MODID
#define MCMUX_DRV_NAME	CONFIG_STREAMS_MCMUX_NAME
#define MCMUX_CMAJORS	CONFIG_STREAMS_MCMUX_NMAJORS
#define MCMUX_CMAJOR_0	CONFIG_STREAMS_MCMUX_MAJOR
#define MCMUX_UNITS	CONFIG_STREAMS_MCMUX_NMINORS

#ifndef MCMUX_DRV_ID
#error "MCMUX_DRV_ID must be defined."
#endif
#ifndef MCMUX_DRV_NAME
#error "MCMUX_DRV_NAME must be defined."
#endif

#define DRV_ID		MCMUX_DRV_ID
#define DRV_NAME	MCMUX_DRV_NAME
#ifdef MODULE
#define DRV_BANNER	MCMUX_BANNER
#else MODULE
#define DRV_BANNER	MCMUX_SPLASH
#endif MODULE

#define FIRST_CMINOR	0
#define MC_CMINOR	0
#define MGR_CMINOR	1
#define TP_CMINOR	2
#define LAST_CMINOR	2

#define FREE_CMINOR	10

#ifndef module_param
MODULE_PARM(modid, "h");
MODULE_PARM(major, "h");
#else
module_param(modid, ushort, 0444);
module_param(major, ushort, 0444);
#endif
MODULE_PARM_DESC(modid, "Module ID for MCMOD.  (0 for allocation.)");
MODULE_PARM_DESC(major, "Device ID for MCMOD.  (0 for allocation.)");

#ifdef LINUX
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_MCMUX_MODID));
MODULE_ALIAS("streams-driver-mcmux");
MODULE_ALIAS("/dev/streams/mcmux");
MODULE_ALIAS("/dev/streams/mcmux/*");
MODULE_ALIAS("/dev/streams/clone/mcmux");
MODULE_ALIAS("/dev/streams/mcmux/mc");
MODULE_ALIAS("/dev/streams/mcmux/mgr");
MODULE_ALIAS("/dev/streams/mcmux/tp");
MODULE_ALIAS("char-major-" __stringify(MCMUX_CMAJOR_0));
MODULE_ALIAS("char-major-" __stringify(MCMUX_CMAJOR_0) "-*");
MODULE_ALIAS("char-major-" __stringify(MCMUX_CMAJOR_0) "-0");
MODULE_ALIAS("char-major-" __stringify(MCMUX_CMAJOR_0) "-" __stringify(MGR_CMINOR));
MODULE_ALIAS("char-major-" __stringify(MCMUX_CMAJOR_0) "-" __stringify(TP_CMINOR));
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

#ifdef _OPTIMIZE_SPEED
#define STRLOG(priv, level, flags, fmt, ...) \
	do { } while (0)
#else
#define STRLOG(priv, level, flags, fmt, ...) \
	mi_strlog(priv->rq, level, flags, fmt, ##__VA_ARGS__)
#endif

#define STRLOGERR	0	/* log error information */
#define STRLOGNO	0	/* log notice information */
#define STRLOGST	1	/* log state transitions */
#define STRLOGTO	2	/* log timeouts */
#define STRLOGRX	3	/* log primitives received */
#define STRLOGTX	4	/* log primitives issued */
#define STRLOGTE	5	/* log timer events */
#define STRLOGIO	6	/* log additional data */
#define STRLOGDA	7	/* log data */

#if 1
#define LOGERR(priv, fmt, ...) STRLOG(priv, STRLOGERR, SL_TRACE | SL_ERROR | SL_CONSOLE, fmt, ##__VA_ARGS__)
#define LOGNO(priv, fmt, ...) STRLOG(priv, STRLOGNO, SL_TRACE, fmt, ##__VA_ARGS__)
#define LOGST(priv, fmt, ...) STRLOG(priv, STRLOGST, SL_TRACE, fmt, ##__VA_ARGS__)
#define LOGTO(priv, fmt, ...) STRLOG(priv, STRLOGTO, SL_TRACE, fmt, ##__VA_ARGS__)
#define LOGRX(priv, fmt, ...) STRLOG(priv, STRLOGRX, SL_TRACE, fmt, ##__VA_ARGS__)
#define LOGTX(priv, fmt, ...) STRLOG(priv, STRLOGTX, SL_TRACE, fmt, ##__VA_ARGS__)
#define LOGTE(priv, fmt, ...) STRLOG(priv, STRLOGTE, SL_TRACE, fmt, ##__VA_ARGS__)
#define LOGIO(priv, fmt, ...) STRLOG(priv, STRLOGIO, SL_TRACE, fmt, ##__VA_ARGS__)
#define LOGDA(priv, fmt, ...) STRLOG(priv, STRLOGDA, SL_TRACE, fmt, ##__VA_ARGS__)
#else
#define LOGERR(priv, fmt, ...) STRLOG(priv, STRLOGERR, SL_TRACE | SL_ERROR | SL_CONSOLE, fmt, ##__VA_ARGS__)
#define LOGNO(priv, fmt, ...) STRLOG(priv, STRLOGNO, SL_TRACE | SL_ERROR | SL_CONSOLE, fmt, ##__VA_ARGS__)
#define LOGST(priv, fmt, ...) STRLOG(priv, STRLOGST, SL_TRACE | SL_ERROR | SL_CONSOLE, fmt, ##__VA_ARGS__)
#define LOGTO(priv, fmt, ...) STRLOG(priv, STRLOGTO, SL_TRACE | SL_ERROR | SL_CONSOLE, fmt, ##__VA_ARGS__)
#define LOGRX(priv, fmt, ...) STRLOG(priv, STRLOGRX, SL_TRACE | SL_ERROR | SL_CONSOLE, fmt, ##__VA_ARGS__)
#define LOGTX(priv, fmt, ...) STRLOG(priv, STRLOGTX, SL_TRACE | SL_ERROR | SL_CONSOLE, fmt, ##__VA_ARGS__)
#define LOGTE(priv, fmt, ...) STRLOG(priv, STRLOGTE, SL_TRACE, fmt, ##__VA_ARGS__)
#define LOGIO(priv, fmt, ...) STRLOG(priv, STRLOGIO, SL_TRACE, fmt, ##__VA_ARGS__)
#define LOGDA(priv, fmt, ...) STRLOG(priv, STRLOGDA, SL_TRACE, fmt, ##__VA_ARGS__)
#endif

/*
 * DEBUGGING
 * --------------------------------------------------------------------------
 */
static const char *
unix_errname(t_scalar_t err)
{
	switch (err) {
	case 0:
		return ("(success)");
	case EPROTO:
		return ("EPROTO");
	case EMSGSIZE:
		return ("EMSGSIZE");
	case EINVAL:
		return ("EINVAL");
	case EOPNOTSUPP:
		return ("EOPNOTSUPP");
	case EPERM:
		return ("EPERM");
	}
	return ("(unknown)");
}
static const char *
unix_errstring(t_scalar_t err)
{
	switch (err) {
	case 0:
		return ("No error");
	case EPROTO:
		return ("Protocol violation");
	case EMSGSIZE:
		return ("Bad message size");
	case EINVAL:
		return ("Invalid argument");
	case EOPNOTSUPP:
		return ("Operation not supported");
	case EPERM:
		return ("No permission");
	}
	return ("Unknown error code.");
}

static const char *
mc_statename(t_scalar_t state)
{
	switch (state) {
	}
	return ("(unknown)");
}
static const char *
mc_primname(t_scalar_t prim)
{
	switch (prim) {
	case MAP_INFO_REQ:
		return ("MAP_INFO_REQ");
	case MAP_BIND_REQ:
		return ("MAP_BIND_REQ");
	case MAP_UNBIND_REQ:
		return ("MAP_UNBIND_REQ");
	case MAP_OPTMGMT_REQ:
		return ("MAP_OPTMGMT_REQ");
	case MAP_OPEN_REQ:
		return ("MAP_OPEN_REQ");
	case MAP_OPEN_RES:
		return ("MAP_OPEN_RES");
	case MAP_REFUSE_RES:
		return ("MAP_REFUSE_RES");
	case MAP_SERV_REQ:
		return ("MAP_SERV_REQ");
	case MAP_SERV_RES:
		return ("MAP_SERV_RES");
	case MAP_DELIM_REQ:
		return ("MAP_DELIM_REQ");
	case MAP_CLOSE_REQ:
		return ("MAP_CLOSE_REQ");
	case MAP_ABORT_REQ:
		return ("MAP_ABORT_REQ");
	case MAP_BIND_ACK:
		return ("MAP_BIND_ACK");
	case MAP_OK_ACK:
		return ("MAP_OK_ACK");
	case MAP_ERROR_ACK:
		return ("MAP_ERROR_ACK");
	case MAP_OPEN_IND:
		return ("MAP_OPEN_IND");
	case MAP_OPEN_CON:
		return ("MAP_OPEN_CON");
	case MAP_REFUSE_IND:
		return ("MAP_REFUSE_IND");
	case MAP_SERV_IND:
		return ("MAP_SERV_IND");
	case MAP_SERV_CON:
		return ("MAP_SERV_CON");
	case MAP_DELIM_IND:
		return ("MAP_DELIM_IND");
	case MAP_CLOSE_IND:
		return ("MAP_CLOSE_IND");
	case MAP_ABORT_IND:
		return ("MAP_ABORT_IND");
	case MAP_NOTICE_IND:
		return ("MAP_NOTICE_IND");
	case MAP_STC1_REQ:
		return ("MAP_STC1_REQ");
	case MAP_STC2_REQ:
		return ("MAP_STC2_REQ");
	case MAP_STC3_REQ:
		return ("MAP_STC3_REQ");
	case MAP_STC4_REQ:
		return ("MAP_STC4_REQ");
	case MAP_STC1_IND:
		return ("MAP_STC1_IND");
	case MAP_STC2_IND:
		return ("MAP_STC2_IND");
	case MAP_STC3_IND:
		return ("MAP_STC3_IND");
	case MAP_STC4_IND:
		return ("MAP_STC4_IND");
	}
	return ("MAP_????");
}
static const char *
mc_iocname(t_scalar_t err)
{
	switch (_IOC_TYPE(cmd)) {
	case __SID:
		switch (_IOC_NR(cmd)) {
		case _IOC_NR(I_LINK):
			return ("I_LINK");
		case _IOC_NR(I_PLINK):
			return ("I_PLINK");
		case _IOC_NR(I_UNLINK):
			return ("I_UNLINK");
		case _IOC_NR(I_PUNLINK):
			return ("I_PUNLINK");
		}
		return ("I_????");
	case TCAP_IOC_MAGIC:
		switch (_IOC_NR(cmd)) {
		case _IOC_NR(TCAP_IOCGOPTION):
			return ("TCAP_IOCGOPTION");
		case _IOC_NR(TCAP_IOCSOPTION):
			return ("TCAP_IOCSOPTION");
		case _IOC_NR(TCAP_IOCGCONFIG):
			return ("TCAP_IOCGCONFIG");
		case _IOC_NR(TCAP_IOCSCONFIG):
			return ("TCAP_IOCSCONFIG");
		case _IOC_NR(TCAP_IOCTCONFIG):
			return ("TCAP_IOCTCONFIG");
		case _IOC_NR(TCAP_IOCCCONFIG):
			return ("TCAP_IOCCCONFIG");
		case _IOC_NR(TCAP_IOCGSTATEM):
			return ("TCAP_IOCGSTATEM");
		case _IOC_NR(TCAP_IOCCMRESET):
			return ("TCAP_IOCCMRESET");
		case _IOC_NR(TCAP_IOCGSTATSP):
			return ("TCAP_IOCGSTATSP");
		case _IOC_NR(TCAP_IOCSSTATSP):
			return ("TCAP_IOCSSTATSP");
		case _IOC_NR(TCAP_IOCGSTATS):
			return ("TCAP_IOCGSTATS");
		case _IOC_NR(TCAP_IOCCSTATS):
			return ("TCAP_IOCCSTATS");
		case _IOC_NR(TCAP_IOCGNOTIFY):
			return ("TCAP_IOCGNOTIFY");
		case _IOC_NR(TCAP_IOCSNOTIFY):
			return ("TCAP_IOCSNOTIFY");
		case _IOC_NR(TCAP_IOCCNOTIFY):
			return ("TCAP_IOCCNOTIFY");
		case _IOC_NR(TCAP_IOCCMGMT):
			return ("TCAP_IOCCMGMT");
		case _IOC_NR(TCAP_IOCCPASS):
			return ("TCAP_IOCCPASS");
		}
		return ("TCAP_IOC????");
	case SCCP_IOC_MAGIC:
		switch (_IOC_NR(cmd)) {
		case _IOC_NR(SCCP_IOCGOPTION):
			return ("SCCP_IOCGOPTION");
		case _IOC_NR(SCCP_IOCSOPTION):
			return ("SCCP_IOCSOPTION");
		case _IOC_NR(SCCP_IOCGCONFIG):
			return ("SCCP_IOCGCONFIG");
		case _IOC_NR(SCCP_IOCSCONFIG):
			return ("SCCP_IOCSCONFIG");
		case _IOC_NR(SCCP_IOCTCONFIG):
			return ("SCCP_IOCTCONFIG");
		case _IOC_NR(SCCP_IOCCCONFIG):
			return ("SCCP_IOCCCONFIG");
		case _IOC_NR(SCCP_IOCGSTATEM):
			return ("SCCP_IOCGSTATEM");
		case _IOC_NR(SCCP_IOCCMRESET):
			return ("SCCP_IOCCMRESET");
		case _IOC_NR(SCCP_IOCGSTATSP):
			return ("SCCP_IOCGSTATSP");
		case _IOC_NR(SCCP_IOCSSTATSP):
			return ("SCCP_IOCSSTATSP");
		case _IOC_NR(SCCP_IOCGSTATS):
			return ("SCCP_IOCGSTATS");
		case _IOC_NR(SCCP_IOCCSTATS):
			return ("SCCP_IOCCSTATS");
		case _IOC_NR(SCCP_IOCGNOTIFY):
			return ("SCCP_IOCGNOTIFY");
		case _IOC_NR(SCCP_IOCSNOTIFY):
			return ("SCCP_IOCSNOTIFY");
		case _IOC_NR(SCCP_IOCCNOTIFY):
			return ("SCCP_IOCCNOTIFY");
		case _IOC_NR(SCCP_IOCCMGMT):
			return ("SCCP_IOCCMGMT");
		case _IOC_NR(SCCP_IOCCPASS):
			return ("SCCP_IOCCPASS");
		}
		return ("SCCP_IOC????");
	case MTP_IOC_MAGIC:
		switch (_IOC_NR(cmd)) {
		case _IOC_NR(MTP_IOCGOPTION):
			return ("MTP_IOCGOPTION");
		case _IOC_NR(MTP_IOCSOPTION):
			return ("MTP_IOCSOPTION");
		case _IOC_NR(MTP_IOCGCONFIG):
			return ("MTP_IOCGCONFIG");
		case _IOC_NR(MTP_IOCSCONFIG):
			return ("MTP_IOCSCONFIG");
		case _IOC_NR(MTP_IOCTCONFIG):
			return ("MTP_IOCTCONFIG");
		case _IOC_NR(MTP_IOCCCONFIG):
			return ("MTP_IOCCCONFIG");
		case _IOC_NR(MTP_IOCGSTATEM):
			return ("MTP_IOCGSTATEM");
		case _IOC_NR(MTP_IOCCMRESET):
			return ("MTP_IOCCMRESET");
		case _IOC_NR(MTP_IOCGSTATSP):
			return ("MTP_IOCGSTATSP");
		case _IOC_NR(MTP_IOCSSTATSP):
			return ("MTP_IOCSSTATSP");
		case _IOC_NR(MTP_IOCGSTATS):
			return ("MTP_IOCGSTATS");
		case _IOC_NR(MTP_IOCCSTATS):
			return ("MTP_IOCCSTATS");
		case _IOC_NR(MTP_IOCGNOTIFY):
			return ("MTP_IOCGNOTIFY");
		case _IOC_NR(MTP_IOCSNOTIFY):
			return ("MTP_IOCSNOTIFY");
		case _IOC_NR(MTP_IOCCNOTIFY):
			return ("MTP_IOCCNOTIFY");
		case _IOC_NR(MTP_IOCCMGMT):
			return ("MTP_IOCCMGMT");
		case _IOC_NR(MTP_IOCCPASS):
			return ("MTP_IOCCPASS");
		}
		return ("MTP_IOC????");
	default:
		return ("????");
	}
}
static const char *
mc_errname(t_scalar_t err)
{
	if (err < 0)
		return unix_errname(-err);
	return ("(unknown)");
}
static const char *
mc_errstring(t_scalar_t err)
{
	if (err < 0)
		return unix_errstring(-err);
	return ("Unknown error code");
}

static const char *
tc_statename(t_scalar_t state)
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
	case TCS_NOSTATES:
		return ("TCS_NOSTATES");
	case -1:
		return ("TCS_FAILED");
	}
	return ("TCS_????");
}
static const char *
tc_primname(t_scalar_t prim)
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
	case TC_COORD_REQ:
		return ("TC_COORD_REQ");
	case TC_COORD_RES:
		return ("TC_COORD_RES");
	case TC_COORD_IND:
		return ("TC_COORD_IND");
	case TC_COORD_CON:
		return ("TC_COORD_CON");
	case TC_STATE_REQ:
		return ("TC_STATE_REQ");
	case TC_STATE_IND:
		return ("TC_STATE_IND");
	case TC_PCSTATE_IND:
		return ("TC_PCSTATE_IND");
	case TC_TRAFFIC_IND:
		return ("TC_TRAFFIC_IND");
	}
	return ("TC_????");
}
static const char *
tc_errname(t_scalar_t err)
{
	if (err < 0)
		return unix_errname(-err);
	switch (err) {
	case TCBADADDR:
		return ("TCBADADDR");
	case TCBADOPT:
		return ("TCBADOPT");
	case TCACCESS:
		return ("TCACCESS");
	case TCNOADDR:
		return ("TCNOADDR");
	case TCOUTSTATE:
		return ("TCOUTSTATE");
	case TCBADSEQ:
		return ("TCBADSEQ");
	case TCSYSERR:
		return ("TCSYSERR");
	case TCBADDATA:
		return ("TCBADDATA");
	case TCBADFLAG:
		return ("TCBADFLAG");
	case TCNOTSUPPORT:
		return ("TCNOTSUPPORT");
	case TCBOUND:
		return ("TCBOUND");
	case TCBADQOSPARAM:
		return ("TCBADQOSPARAM");
	case TCBADQOSTYPE:
		return ("TCBADQOSTYPE");
	case TCBADTOKEN:
		return ("TCBADTOKEN");
	case TCNOPROTOID:
		return ("TCNOPROTOID");
	}
	return ("(unknown)");
}
static const char *
tc_errstring(t_scalar_t err)
{
	if (err < 0)
		return unix_errstring(-err);
	switch (err) {
	case TCBADADDR:
		return ("Incorrect address format/illegal address information");
	case TCBADOPT:
		return ("Options in incorrect format or contain illegal information");
	case TCACCESS:
		return ("User did not have proper permissions");
	case TCNOADDR:
		return ("TC Provider could not allocate address");
	case TCOUTSTATE:
		return ("Primitive was issues in wrong sequence");
	case TCBADSEQ:
		return ("Sequence number in primitive was incorrect/illegal");
	case TCSYSERR:
		return ("UNIX system error occurred");
	case TCBADDATA:
		return ("User data spec. outside range supported by TC provider");
	case TCBADFLAG:
		return ("Flags specified in primitive were illegal/incorrect");
	case TCNOTSUPPORT:
		return ("Primitive type not supported by the TC provider");
	case TCBOUND:
		return ("Illegal second attempt to bind listener or default listener");
	case TCBADQOSPARAM:
		return ("QOS values specified are outside the range supported by the TC provider");
	case TCBADQOSTYPE:
		return ("QOS structure type specified is not supported by the TC provider");
	case TCBADTOKEN:
		return ("Token used is not associated with an open stream");
	case TCNOPROTOID:
		return ("Protocol id could not be allocated");
	}
	return ("Unknown error code");
}

/*
 * STREAMS DEFINITIONS
 * --------------------------------------------------------------------------
 */
static struct module_info mc_minfo = {
	.mi_idnum = MOD_ID,
	.mi_idname = MOD_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_stat tc_mstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat mc_mstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

static rwlock_t mc_lock = RW_LOCK_UNLOCKED;
static caddr_t mc_opens = NULL;
static caddr_t mc_links = NULL;

/*
 * PRIVATE STRUCTURES
 * --------------------------------------------------------------------------
 */
struct mc;
struct tc;
struct dg;

struct t_xti_options {
	t_scalar_t debug[4];
	struct t_linger linger;
	t_uscalar_t rcvbuf;
	t_uscalar_t rcvlowat;
	t_uscalar_t sndbuf;
	t_uscalar_t sndlowat;
};
struct t_mtp_options {
	t_uscalar_t pvar;
	t_uscalar_t mplev;
	t_uscalar_t sls;
	t_uscalar_t mp;
};
struct t_sccp_options {
	t_uscalar_t pvar;
	t_uscalar_t mplev;
	t_uscalar_t cluster;
	t_uscalar_t prio;
	t_uscalar_t pclass;
	t_uscalar_t seqctrl;
	t_uscalar_t imp;
	t_uscalar_t reterr;
	t_uscalar_t credit;
	struct sockaddr_sccp discon_add;
	t_uscalar_t discon_reason;
	t_uscalar_t reset_reason;
	N_qos_sccp_t qos;
};
struct t_tcap_options {
	t_uscalar_t pvar;
};
struct t_map_options {
	t_uscalar_t pvar;
};

struct t_mc_options {
	long flags[4];
	struct t_xti_options xti;
	struct t_mtp_options mtp;
	struct t_sccp_options sccp;
	struct t_tcap_options tcap;
	struct t_map_options mc;
};

struct dg {
	struct mc *mc;
	struct dg *dg_next;
	struct dg **dg_prev;
	t_scalar_t CURRENT_state;
	t_uscalar_t pvar;
	struct {
		t_scalar_t CURRENT_state;
	} save;
	struct {
		t_scalar_t DIALOG_id;
		t_scalar_t ASSOC_flags;
		t_scalar_t ADDR_length;
		t_scalar_t ADDR_offset;
		union {
			struct sockaddr_sccp addr;
			char abuf[sizeof(struct sockaddr_sccp)];
		};
		union {
			N_qos_sel_data_sccp_t qos;
			char qbuf[sizeof(N_qos_sel_data_sccp_t)];
		};
	} info, peer;
};

struct common {
	struct mc *mc;
	struct tc *tc;
};

struct mc {
	struct common self;
	queue_t *rq, *wq;
	cred_t cred;
	dev_t dev;
	struct tc *tc;
	uint pvar;
	t_scalar_t base;
	t_scalar_t increment;
	t_scalar_t DIALOG_id;
	t_uscalar_t DIAIND_number;
	struct dg *dg_hash[256];
	struct {
		t_scalar_t DIALOG_id;
		t_scalar_t CURRENT_state;
	} save;
	struct {
		t_scalar_t TSDU_size;
		t_scalar_t ETSDU_size;
		t_scalar_t CDATA_size;
		t_scalar_t DDATA_size;
		t_scalar_t ADDR_size;
		t_scalar_t OPT_size;
		t_scalar_t TIDU_size;
		t_scalar_t SERV_type;
		t_scalar_t CURRENT_state;
		t_scalar_t PROVIDER_flag;
		t_scalar_t MAP_version;
	} info;
	struct {
		t_scalar_t DIAIND_number;
		t_scalar_t BIND_flags;
		t_scalar_t ADDR_length;
		t_scalar_t ADDR_offset;
		union {
			struct sockaddr_sccp addr;
			char abuf[sizeof(struct sockaddr_sccp)];
		};
	} bind;
};

struct tc {
	struct common self;
	queue_t *rq, *wq;
	cred_t cred;
	dev_t dev;
	struct mc *mc;
	uint pvar;
	t_scalar_t base;
	t_scalar_t increment;
	t_scalar_t DIALOG_id;
	t_uscalar_t DIAIND_number;
	struct dg *dg_hash[256];
	struct {
		t_scalar_t DIALOG_id;
		t_scalar_t CURRENT_state;
	} save;
	struct {
		t_scalar_t TSDU_size;
		t_scalar_t ETSDU_size;
		t_scalar_t CDATA_size;
		t_scalar_t DDATA_size;
		t_scalar_t ADDR_size;
		t_scalar_t OPT_size;
		t_scalar_t TIDU_size;
		t_scalar_t SERV_type;
		t_scalar_t CURRENT_state;
		t_scalar_t PROVIDER_flag;
		t_scalar_t TCI_version;
	} info;
	struct {
		t_scalar_t DIAIND_number;
		t_scalar_t BIND_flags;
		t_scalar_t ADDR_length;
		t_scalar_t ADDR_offset;
		union {
			struct sockaddr_sccp addr;
			char abuf[sizeof(struct sockaddr_sccp)];
		};
	} bind;
};

struct priv {
	struct mc mc;
	struct tc tc;
};

#define PRIV(q) = ((struct priv *)q->q_ptr)
#define MC_PRIV(q) = (PRIV(q)->mc.self.mc)
#define TC_PRIV(q) = (PRIV(q)->mc.self.tc)

/*
 * LOCKING
 * --------------------------------------------------------------------------
 */
static inline fastcall struct mc *
mc_trylock(queue_t *q)
{
	struct priv *priv;

	read_lock(&mc_lock);
	priv = (struct priv *) mi_trylock(q);
	read_unlock(&mc_lock);
	return (priv->self.mc);
}
static inline fastcall void
mc_unlock(struct mc *mc)
{
	read_lock(&mc_lock);
	mi_unlock(mc->ptr);
	read_unlock(&mc_lock);
}

static inline fastcall struct tc *
tc_trylock(queue_t *q)
{
	struct priv *priv;

	read_lock(&tc_lock);
	priv = (struct priv *) mi_trylock(q);
	read_unlock(&tc_lock);
	return (priv->tc);
}
static inline fastcall void
tc_unlock(struct tc *tc)
{
	read_lock(&tc_lock);
	mi_unlock(tc->ptr);
	read_unlock(&tc_lock);
}

static fastcall int
mc_get_lower(struct mc *mc, queue_t *q, struct tc **tcp)
{
	struct tc *tc;
	int err;

	if (!(tc = mc->tc))
		goto outstate;
	if (tc != mc->self.tc) {
		read_lock(&mc_lock);
		if (!mi_acquire(tc->ptr, q)) {
			read_unlock(&mc_lock);
			goto edeadlk;
		}
		read_unlock(&mc_lock);
	}
	*tcp = tc;
	return (0);
      edeadlk:
	err = EDEADLK;
	goto error;
      outstate:
	err = MAPOUTSTATE;
	goto error;
      error:
	return (err);
}
static fastcall void
mc_put_lower(struct mc *mc, struct tc *tc)
{
	if (tc != NULL && tc != mc->self.tc)
		mi_release(tc->ptr);
}

static fastcall int
tc_get_upper(struct tc *tc, queue_t *q, struct mc **mcp)
{
	struct mc *mc;
	int err;

	if (!(mc = tc->mc))
		goto outstate;
	if (mc != tc->self.mc) {
		read_lock(&tc_lock);
		if (!mi_acquire(mc->ptr, q)) {
			read_unlock(&tc_lock);
			goto edeadlk;
		}
		read_unlock(&tc_lock);
	}
	*mcp = mc;
	return (0);
      edeadlk:
	err = EDEADLK;
	goto error;
      outstate:
	err = MAPOUTSTATE;
	goto error;
      error:
	return (err);
}
static fastcall void
tc_put_upper(struct tc *tc, struct mc *mc)
{
	if (mc != NULL && mc != tc->self.mc)
		mi_release(mc->ptr);
}

/*
 * STATE FUNCTIONS
 * --------------------------------------------------------------------------
 */

static t_scalar_t
mc_save_state(struct mc *mc)
{
	mc->save.DIALOG_id = mc->info.DIALOG_id;
	return (mc->save.CURRENT_state = mc->info.CURRENT_state);
}
static t_scalar_t
mc_restore_state(struct mc *mc)
{
	mc->info.DIALOG_id = mc->save.DIALOG_id;
	return (mc->info.CURRENT_state = mc->save.CURRENT_state);
}
static inline fastcall t_scalar_t
mc_get_state(struct mc *mc)
{
	return (mc->info.CURRENT_state);
}
static inline fastcall t_scalar_t
mc_set_state(struct mc *mc, t_scalar_t newstate)
{
	t_scalar_t oldstate = mc->info.CURRENT_state;

	if (newstate != oldstate) {
		LOGST(mc, "%s <- %s", mc_statename(newstate), mc_statename(oldstate));
		mc->info.CURRENT_state = newstate;
	}
	return (oldstate);
}

static t_scalar_t
tc_save_state(struct tc *tc)
{
	tc->save.DIALOG_id = tc->info.DIALOG_id;
	return (tc->save.CURRENT_state = tc->info.CURRENT_state);
}
static t_scalar_t
mc_restore_state(struct tc *tc)
{
	tc->info.DIALOG_id = tc->save.DIALOG_id;
	return (tc->info.CURRENT_state = tc->save.CURRENT_state);
}
static inline fastcall t_scalar_t
mc_get_state(struct tc *tc)
{
	return (tc->info.CURRENT_state);
}
static inline fastcall t_scalar_t
mc_set_state(struct tc *tc, t_scalar_t newstate)
{
	t_scalar_t oldstate = tc->info.CURRENT_state;

	if (newstate != oldstate) {
		LOGST(tc, "%s <- %s", mc_statename(newstate), mc_statename(oldstate));
		tc->info.CURRENT_state = newstate;
	}
	return (oldstate);
}

/*
 * PRIVATE STRUCTURE CACHES
 * --------------------------------------------------------------------------
 */
static kmem_cachep_t priv_cachep = NULL;
static kmem_cachep_t mc_cachep = NULL;
static kmem_cachep_t tc_cachep = NULL;
static kmem_cachep_t dg_cachep = NULL;

static __unlikely int
mc_term_caches(void)
{
	if (dg_cachep) {
#if defined HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(dg_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy dg_cachep", __FUNCTION__);
			return (-EBUSY);
		} else
			cmn_err(CE_DEBUG, "%s: destroyed dg_cachep", MOD_NAME);
#else				/* !defined HAVE_KTYPE_KMEM_CACHE_T_P */
		kmem_cache_destroy(dg_cachep);
#endif				/* !defined HAVE_KTYPE_KMEM_CACHE_T_P */
	}
	if (tc_cachep) {
#if defined HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(tc_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy tc_cachep", __FUNCTION__);
			return (-EBUSY);
		} else
			cmn_err(CE_DEBUG, "%s: destroyed tc_cachep", MOD_NAME);
#else				/* !defined HAVE_KTYPE_KMEM_CACHE_T_P */
		kmem_cache_destroy(tc_cachep);
#endif				/* !defined HAVE_KTYPE_KMEM_CACHE_T_P */
	}
	if (mc_cachep) {
#if defined HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(mc_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy mc_cachep", __FUNCTION__);
			return (-EBUSY);
		} else
			cmn_err(CE_DEBUG, "%s: destroyed mc_cachep", MOD_NAME);
#else				/* !defined HAVE_KTYPE_KMEM_CACHE_T_P */
		kmem_cache_destroy(mc_cachep);
#endif				/* !defined HAVE_KTYPE_KMEM_CACHE_T_P */
	}
	if (priv_cachep) {
#if defined HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(priv_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy priv_cachep", __FUNCTION__);
			return (-EBUSY);
		} else
			cmn_err(CE_DEBUG, "%s: destroyed priv_cachep", MOD_NAME);
#else				/* !defined HAVE_KTYPE_KMEM_CACHE_T_P */
		kmem_cache_destroy(priv_cachep);
#endif				/* !defined HAVE_KTYPE_KMEM_CACHE_T_P */
	}
	return (0);
}

static __unlikely int
mc_init_caches(void)
{
	if (!priv_cachep
	    && !(priv_cachep =
		 kmem_create_cache("priv_cachep", mi_open_size(sizeof(struct priv)), 0,
				   SLAB_HWCACHE_ALIGN, NULL, NULL)
	    )) {
		cmn_err(CE_PANIC, "%s: could not allocate priv_cachep", __FUNCTION__);
		mc_term_caches();
		return (-ENOMEM);
	} else
		cmn_err(CE_DEBUG, "%s: initialized driver private structure cache", MOD_NAME);
	if (!mc_cachep
	    && !(mc_cachep =
		 kmem_create_cache("mc_cachep", mi_open_size(sizeof(struct mc)), 0,
				   SLAB_HWCACHE_ALIGN, NULL, NULL)
	    )) {
		cmn_err(CE_PANIC, "%s: could not allocate mc_cachep", __FUNCTION__);
		tr_term_caches();
		return (-ENOMEM);
	} else
		cmn_err(CE_DEBUG, "%s: initialized driver mc structure cache", MOD_NAME);
	if (!tc_cachep
	    && !(tc_cachep =
		 kmem_create_cache("tc_cachep", mi_open_size(sizeof(struct tc)), 0,
				   SLAB_HWCACHE_ALIGN, NULL, NULL)
	    )) {
		cmn_err(CE_PANIC, "%s: could not allocate tc_cachep", __FUNCTION__);
		tr_term_caches();
		return (-ENOMEM);
	} else
		cmn_err(CE_DEBUG, "%s: initialized driver tc structure cache", MOD_NAME);
	if (!dg_cachep
	    && !(dg_cachep =
		 kmem_create_cache("dg_cachep", sizeof(struct dg), 0,
				   SLAB_HWCACHE_ALIGN, NULL, NULL)
	    )) {
		cmn_err(CE_PANIC, "%s: could not allocate dg_cachep", __FUNCTION__);
		tr_term_caches();
		return (-ENOMEM);
	} else
		cmn_err(CE_DEBUG, "%s: initialized driver dg structure cache", MOD_NAME);
}

/*
 * CONSTRUCTORS AND DESTRUCTORS
 * --------------------------------------------------------------------------
 */

#define dg_hash_function(DIALOG_id) ((DIALOG_id ^ (DIALOG_id >> 8) ^ (DIALOG_id >> 16)) & (0x7f))

static inline struct dg *
mc_lookup(struct mc *mc, t_scalar_t DIALOG_id)
{
	struct dg *dg;

	for (dg = mc->dg_hash[dg_hash_function(DIALOG_id)]; dg; dg = dg->dg_next)
		if (dg->info.DIALOG_id == DIALOG_id)
			break;
	return (dg);
}

static inline t_scalar_t
mc_next_dialogid(struct mc *mc, t_scalar_t DIALOG_id)
{
	if (!(DIALOG_id += mc->increment))
		DIALOG_id += mc->increment;
	if (DIALOG_id < 0)
		DIALOG_id = mc->base;
	if (!DIALOG_id)
		DIALOG_id += mc->increment;
	return (DIALOG_id);
}

static inline int
mc_alloc_dialogid(struct mc *mc, t_scalar_t *tidp)
{
	t_scalar_t DIALOG_id = *tidp;
	int err;

	if (DIALOG_id == 0) {
		int i;

		DIALOG_id = mc_next_dialogid(mc, mc->DIALOG_id);
		for (i = 128; i > 0; i--) {
			if (!mc_lookup(mc, DIALOG_id))
				break;
			DIALOG_id = mc_next_dialogid(mc, DIALOG_id);
		}
		if (i == 0)
			goto noprotoid;
		mc->DIALOG_id = DIALOG_id;
		*tidp = DIALOG_id;
	} else if (mc_lookup(mc, DIALOG_id))
		goto badseq;
	return (0);
      noprotoid:
	err = TCNOPROTOID;
	goto error;
      badseq:
	err = TCBADSEQ;
	goto error;
      error:
	return (err);
}

static fastcall int
dg_create(struct tc *tc, struct dg **dgp, t_scalar_t DIALOG_id)
{
	struct dg *dg;
	int hash;
	int err;

	if ((err = tc_alloc_dialogid(tc, &DIALOG_id)))
		goto error;
	if (!(dg = kmem_cache_alloc(dg_cachep, GFP_ATOMIC)))
		goto enomem;
	bzero(dg, sizeof(*dg));
	tc->DIAIND_number++;
	dg->tc = tc;
	hash = dg_hash_function(DIALOG_id);
	if ((dg->dg_next = tc->dg_hash[hash]))
		dg->dg_next->dg_prev = &dg->dg_next;
	dg->dg_prev = &tc->dg_hash[hash];
	dg->CURRENT_state = TRS_IDLE;
	dg->pvar = tc->pvar;
	dg->info.DIALOG_id = DIALOG_id;
	dg->info.ASSOC_flags = 0;
	dg->info.ADDR_length = 0;
	dg->info.ADDR_offset = &dg->info.abuf - (caddr_t) &dg->info;
	dg->info.qos.n_qos_type = N_QOS_SEL_DATA_SCCP;
	dg->infinfo.qos.protocol_class = QOS_UNKNOWN;
	dg->info.qos.option_flags = QOS_UNKNOWN;
	dg->info.qos.sequence_control = QOS_UNKNOWN;
	dg->info.qos.message_priority = QOS_UNKNOWN;
	dg->info.qos.importance = QOS_UNKNOWN;
	dg->peer.DIALOG_id = 0;
	dg->peer.ASSOC_flags = 0;
	dg->peer.ADDR_length = 0;
	dg->peer.ADDR_offset = &dg->peer.abuf - (caddr_t) &dg->peer;
	dg->peer.qos.n_qos_type = N_QOS_SEL_DATA_SCCP;
	dg->peer.qos.protocol_class = QOS_UNKNOWN;
	dg->peer.qos.option_flags = QOS_UNKNOWN;
	dg->peer.qos.sequence_control = QOS_UNKNOWN;
	dg->peer.qos.message_priority = QOS_UNKNOWN;
	dg->peer.qos.importance = QOS_UNKNOWN;
	*dgp = dg;
	return (0);
      enomem:
	err = -ENOMEM;
	goto error;
      error:
	return (err);
}

static fastcall void
dg_destroy(struct dg *dg)
{
	if (dg != NULL) {
		if ((*dg->dg_prev = dg->dg_next))
			dg->dg_next->dg_prev = dg->dg_prev;
		dg->tc->DIAIND_number--;
		bzero(dg, sizeof(*dg));
		kmem_cache_free(dg_cachep, dg);
	}
}

static inline int
dg_abort(struct dg *dg, queue_t *q)
{
	struct tc *tc = dg->tc;
	int err;

	if (tc_get_state(tc) != NS_IDLE) {
		switch (dg_get_state(dg)) {
		case TRS_WACK_CREQ:
		case TRS_WCON_CREQ:
		case TRS_WRES_CIND:
		case TRS_WACK_CRES:
		case TRS_DATA_XFER:
		case TRS_WACK_DREQ6:
		case TRS_WACK_DREQ7:
		case TRS_WACK_DREQ9:
			if ((err = dg_send_u_abort(tc, dg, q, NULL)))
				goto error;
			dg->CURRENT_state = TRS_IDLE;
			break;
		case TRS_IDLE:
			break;
		}
	}
	dg_destroy(dg);
	return (0);
      enobufs:
	err = -ENOBUFS;
	goto error;
      error:
	return (err);
}

static inline int
tc_abort_transactions(struct tc *tc, queue_t *q)
{
	struct dg *dg;
	uint hash;
	int err;

	if (tc->DIAIND_number) {
		for (hash = 0; hash < 256; hash++) {
			while ((dg = tc->dg_hash[hash])) {
				if ((err = dg_abort(dg, q)))
					goto error;
			}
		}
		tc->DIAIND_number = 0;
	}
	return (0);
      error:
	return (err);
}

static void
tc_init(struct tc *tc, struct tr *tr)
{
	tc->self.tc = tc;
	if ((tc->self.tr = tr)) {
		tc->rq = tr->rq;
		tc->wq = tr->wq;
		tc->cred = tr->cred;
		tc->muxid = tr->dev;
	}
	tc->tr = tr;
	tc->save.CURRENT_state = -1U;
	tc->info.NSDU_size =;
	tc->info.ENSDU_size =;
	tc->info.CDATA_size =;
	tc->info.DDATA_size =;
	tc->info.ADDR_size = sizeof(struct sockaddr_sccp);
	tc->info.ADDR_length = 0;
	tc->info.ADDR_offset = tc->info.abuf - (caddr_t) &tc->info;
	bzero(tc->bind.abuf, sizeof(tc->bind.abuf));
	tc->info.QOS_length = 0;
	tc->info.QOS_offset = tc->info.qbuf - (caddr_t) &tc->info;
	bzero(tc->bind.qbuf, sizeof(tc->bind.qbuf));
	tc->info.QOS_range_length = 0;
	tc->info.QOS_range_offset = tc->info.rbuf - (caddr_t) &tc->info;
	bzero(tc->bind.rbuf, sizeof(tc->bind.rbuf));
	tc->info.OPTIONS_flags = 0;
	tc->info.NIDU_size =;
	tc->info.SERV_type = N_CLNS;
	tc->info.CURRENT_state = -1U;
	tc->info.PROVIDER_type = N_SUBNET;
	tc->info.NODU_size =;
	tc->info.PROTOID_length = 0;
	tc->info.PROTOID_offset = tc->info.pbuf - (caddr_t) &tc->info;
	bzero(tc->bind.pbuf, sizeof(tc->bind.pbuf));
	tc->info.NPI_version = N_CURRENT_VERSION;
	tc->bind.CONIND_number = 0;
	tc->bind.BIND_flags = 0;
	tc->bind.ADDR_length = 0;
	tc->bind.ADDR_offset = tc->bind.abuf - (caddr_t) &tc->bind;
	bzero(tc->bind.abuf, sizeof(tc->bind.abuf));
}

static void
mc_init(struct mc *mc, struct tc *tc)
{
	mc->self.mc = mc;
	mc->self.tc = tc;
	mc->tc = tc;
	mc->base = 0;
	mc->increment = 1;
	mc->TRANS_id = mc->base;
	mc->XACT_number = 0;
	bzero(mc->tn_hash, sizeof(mc->tn_hash));
	mc->save.TRANS_id = mc->base;
	mc->save.CURRENT_state = TRS_UNBND;
	mc->info.TSDU_size =;
	mc->info.ETSDU_size =;
	mc->info.CDATA_size =;
	mc->info.DDATA_size =;
	mc->info.ADDR_size = sizeof(struct sockaddr_sccp);
	mc->info.OPT_size =;
	mc->info.TIDU_size =;
	mc->info.SERV_type =;
	mc->info.CURRENT_state = TRS_UNBND;
	mc->info.PROVIDER_flag =;
	mc->info.TRPI_version = TRPI_CURRENT_VERSION;
	mc->bind.XACT_number = 0;
	mc->bind.BIND_flags = 0;
	mc->bind.ADDR_length = 0;
	mc->bind.ADDR_offset = mc->bind.abuf - (caddr_t) &mc->bind;
	bzero(mc->bind.abuf, sizeof(mc->bind.abuf));
	if (tc)
		tc_init(tc, mc);
	return;
}

static void
priv_init(struct priv *pp)
{
	struct mc *mc = &pp->mc;
	struct tc *tc = &pp->tc;

	mc_init(mc, tc);
}


/*
 * OPTIONS HANDLING
 * --------------------------------------------------------------------------
 */

enum {
	_T_BIT_XTI_DEBUG,
	_T_BIT_XTI_LINGER,
	_T_BIT_XTI_RCVBUF,
	_T_BIT_XTI_RCVLOWAT,
	_T_BIT_XTI_SNDBUF,
	_T_BIT_XTI_SNDLOWAT,
	_T_BIT_MTP_PVAR,
	_T_BIT_MTP_MPLEV,
	_T_BIT_MTP_SLS,
	_T_BIT_MTP_MP,
	_T_BIT_SCCP_PVAR,
	_T_BIT_SCCP_MPLEV,
	_T_BIT_SCCP_CLUSTER,
	_T_BIT_SCCP_PRIO,
	_T_BIT_SCCP_PCLASS,
	_T_BIT_SCCP_SEQCTRL,
	_T_BIT_SCCP_IMP,
	_T_BIT_SCCP_RETERR,
	_T_BIT_SCCP_CREDIT,
	_T_BIT_SCCP_DISCON_ADD,
	_T_BIT_SCCP_DISCON_REASON,
	_T_BIT_SCCP_RESET_REASON,
	_T_BIT_SCCP_QOS,
	_T_BIT_TCAP_PVAR,
	_T_BIT_MAP_PVAR,
};

#define T_SPACE(len) \
	(sizeof(struct t_opthdr) + T_ALIGN(len))

#define T_LENGTH(len) \
	(sizeof(struct t_opthdr) + len)

#define _T_SPACE_SIZEOF(s) \
	T_SPACE(sizeof(s))

#define _T_LENGTH_SIZEOF(s) \
	T_LENGTH(sizeof(s))

struct t_mc_options t_defaults = {
};


/*
 * MAP Provider -> MAP User Primitives
 * --------------------------------------------------------------------------
 */

static fastcall int
mc_bind_ack(struct mc *mc, queue_t *q, mblk_t *msg)
{
	struct MAP_bind_ack *p;
	mblk_t *mp;
	int err;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!bcanputnext(mc->rq, 0)))
		goto ebusy;
	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = MAP_BIND_ACK;
	mp->b_wptr += sizeof(*p);
	freemsg(msg);
	LOGTX(mc, "<- MAP_BIND_ACK");
	putnext(mc->rq, mp);
	return (0);
      ebusy:
	err = -EBUSY;
	goto error;
      enobufs:
	err = -ENOBUFS;
	goto error;
      error:
	return (err);
}
static fastcall int
mc_ok_ack(struct mc *mc, queue_t *q, mblk_t *msg)
{
	struct MAP_bind_ack *p;
	mblk_t *mp;
	int err;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!bcanputnext(mc->rq, 0)))
		goto ebusy;
	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = MAP_BIND_ACK;
	mp->b_wptr += sizeof(*p);
	freemsg(msg);
	LOGTX(mc, "<- MAP_BIND_ACK");
	putnext(mc->rq, mp);
	return (0);
      ebusy:
	err = -EBUSY;
	goto error;
      enobufs:
	err = -ENOBUFS;
	goto error;
      error:
	return (err);
}
static fastcall int
mc_error_ack(struct mc *mc, queue_t *q, mblk_t *msg)
{
	struct MAP_bind_ack *p;
	mblk_t *mp;
	int err;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!bcanputnext(mc->rq, 0)))
		goto ebusy;
	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = MAP_BIND_ACK;
	mp->b_wptr += sizeof(*p);
	freemsg(msg);
	LOGTX(mc, "<- MAP_BIND_ACK");
	putnext(mc->rq, mp);
	return (0);
      ebusy:
	err = -EBUSY;
	goto error;
      enobufs:
	err = -ENOBUFS;
	goto error;
      error:
	return (err);
}
static fastcall int
mc_open_ind(struct mc *mc, queue_t *q, mblk_t *msg)
{
	struct MAP_bind_ack *p;
	mblk_t *mp;
	int err;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!bcanputnext(mc->rq, 0)))
		goto ebusy;
	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = MAP_BIND_ACK;
	mp->b_wptr += sizeof(*p);
	freemsg(msg);
	LOGTX(mc, "<- MAP_BIND_ACK");
	putnext(mc->rq, mp);
	return (0);
      ebusy:
	err = -EBUSY;
	goto error;
      enobufs:
	err = -ENOBUFS;
	goto error;
      error:
	return (err);
}
static fastcall int
mc_open_con(struct mc *mc, queue_t *q, mblk_t *msg)
{
	struct MAP_bind_ack *p;
	mblk_t *mp;
	int err;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!bcanputnext(mc->rq, 0)))
		goto ebusy;
	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = MAP_BIND_ACK;
	mp->b_wptr += sizeof(*p);
	freemsg(msg);
	LOGTX(mc, "<- MAP_BIND_ACK");
	putnext(mc->rq, mp);
	return (0);
      ebusy:
	err = -EBUSY;
	goto error;
      enobufs:
	err = -ENOBUFS;
	goto error;
      error:
	return (err);
}
static fastcall int
mc_refuse_ind(struct mc *mc, queue_t *q, mblk_t *msg)
{
	struct MAP_bind_ack *p;
	mblk_t *mp;
	int err;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!bcanputnext(mc->rq, 0)))
		goto ebusy;
	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = MAP_BIND_ACK;
	mp->b_wptr += sizeof(*p);
	freemsg(msg);
	LOGTX(mc, "<- MAP_BIND_ACK");
	putnext(mc->rq, mp);
	return (0);
      ebusy:
	err = -EBUSY;
	goto error;
      enobufs:
	err = -ENOBUFS;
	goto error;
      error:
	return (err);
}
static fastcall int
mc_serv_ind(struct mc *mc, queue_t *q, mblk_t *msg)
{
	struct MAP_bind_ack *p;
	mblk_t *mp;
	int err;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!bcanputnext(mc->rq, 0)))
		goto ebusy;
	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = MAP_BIND_ACK;
	mp->b_wptr += sizeof(*p);
	freemsg(msg);
	LOGTX(mc, "<- MAP_BIND_ACK");
	putnext(mc->rq, mp);
	return (0);
      ebusy:
	err = -EBUSY;
	goto error;
      enobufs:
	err = -ENOBUFS;
	goto error;
      error:
	return (err);
}
static fastcall int
mc_serv_con(struct mc *mc, queue_t *q, mblk_t *msg)
{
	struct MAP_bind_ack *p;
	mblk_t *mp;
	int err;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!bcanputnext(mc->rq, 0)))
		goto ebusy;
	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = MAP_BIND_ACK;
	mp->b_wptr += sizeof(*p);
	freemsg(msg);
	LOGTX(mc, "<- MAP_BIND_ACK");
	putnext(mc->rq, mp);
	return (0);
      ebusy:
	err = -EBUSY;
	goto error;
      enobufs:
	err = -ENOBUFS;
	goto error;
      error:
	return (err);
}
static fastcall int
mc_delim_ind(struct mc *mc, queue_t *q, mblk_t *msg)
{
	struct MAP_bind_ack *p;
	mblk_t *mp;
	int err;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!bcanputnext(mc->rq, 0)))
		goto ebusy;
	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = MAP_BIND_ACK;
	mp->b_wptr += sizeof(*p);
	freemsg(msg);
	LOGTX(mc, "<- MAP_BIND_ACK");
	putnext(mc->rq, mp);
	return (0);
      ebusy:
	err = -EBUSY;
	goto error;
      enobufs:
	err = -ENOBUFS;
	goto error;
      error:
	return (err);
}
static fastcall int
mc_close_ind(struct mc *mc, queue_t *q, mblk_t *msg)
{
	struct MAP_bind_ack *p;
	mblk_t *mp;
	int err;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!bcanputnext(mc->rq, 0)))
		goto ebusy;
	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = MAP_BIND_ACK;
	mp->b_wptr += sizeof(*p);
	freemsg(msg);
	LOGTX(mc, "<- MAP_BIND_ACK");
	putnext(mc->rq, mp);
	return (0);
      ebusy:
	err = -EBUSY;
	goto error;
      enobufs:
	err = -ENOBUFS;
	goto error;
      error:
	return (err);
}
static fastcall int
mc_abort_ind(struct mc *mc, queue_t *q, mblk_t *msg)
{
	struct MAP_bind_ack *p;
	mblk_t *mp;
	int err;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!bcanputnext(mc->rq, 0)))
		goto ebusy;
	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = MAP_BIND_ACK;
	mp->b_wptr += sizeof(*p);
	freemsg(msg);
	LOGTX(mc, "<- MAP_BIND_ACK");
	putnext(mc->rq, mp);
	return (0);
      ebusy:
	err = -EBUSY;
	goto error;
      enobufs:
	err = -ENOBUFS;
	goto error;
      error:
	return (err);
}
static fastcall int
mc_notice_ind(struct mc *mc, queue_t *q, mblk_t *msg)
{
	struct MAP_bind_ack *p;
	mblk_t *mp;
	int err;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!bcanputnext(mc->rq, 0)))
		goto ebusy;
	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = MAP_BIND_ACK;
	mp->b_wptr += sizeof(*p);
	freemsg(msg);
	LOGTX(mc, "<- MAP_BIND_ACK");
	putnext(mc->rq, mp);
	return (0);
      ebusy:
	err = -EBUSY;
	goto error;
      enobufs:
	err = -ENOBUFS;
	goto error;
      error:
	return (err);
}
static fastcall int
mc_stc1_ind(struct mc *mc, queue_t *q, mblk_t *msg)
{
	struct MAP_bind_ack *p;
	mblk_t *mp;
	int err;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!bcanputnext(mc->rq, 0)))
		goto ebusy;
	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = MAP_BIND_ACK;
	mp->b_wptr += sizeof(*p);
	freemsg(msg);
	LOGTX(mc, "<- MAP_BIND_ACK");
	putnext(mc->rq, mp);
	return (0);
      ebusy:
	err = -EBUSY;
	goto error;
      enobufs:
	err = -ENOBUFS;
	goto error;
      error:
	return (err);
}
static fastcall int
mc_stc2_ind(struct mc *mc, queue_t *q, mblk_t *msg)
{
	struct MAP_bind_ack *p;
	mblk_t *mp;
	int err;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!bcanputnext(mc->rq, 0)))
		goto ebusy;
	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = MAP_BIND_ACK;
	mp->b_wptr += sizeof(*p);
	freemsg(msg);
	LOGTX(mc, "<- MAP_BIND_ACK");
	putnext(mc->rq, mp);
	return (0);
      ebusy:
	err = -EBUSY;
	goto error;
      enobufs:
	err = -ENOBUFS;
	goto error;
      error:
	return (err);
}
static fastcall int
mc_stc3_ind(struct mc *mc, queue_t *q, mblk_t *msg)
{
	struct MAP_bind_ack *p;
	mblk_t *mp;
	int err;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!bcanputnext(mc->rq, 0)))
		goto ebusy;
	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = MAP_BIND_ACK;
	mp->b_wptr += sizeof(*p);
	freemsg(msg);
	LOGTX(mc, "<- MAP_BIND_ACK");
	putnext(mc->rq, mp);
	return (0);
      ebusy:
	err = -EBUSY;
	goto error;
      enobufs:
	err = -ENOBUFS;
	goto error;
      error:
	return (err);
}
static fastcall int
mc_stc4_ind(struct mc *mc, queue_t *q, mblk_t *msg)
{
	struct MAP_bind_ack *p;
	mblk_t *mp;
	int err;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!bcanputnext(mc->rq, 0)))
		goto ebusy;
	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = MAP_BIND_ACK;
	mp->b_wptr += sizeof(*p);
	freemsg(msg);
	LOGTX(mc, "<- MAP_BIND_ACK");
	putnext(mc->rq, mp);
	return (0);
      ebusy:
	err = -EBUSY;
	goto error;
      enobufs:
	err = -ENOBUFS;
	goto error;
      error:
	return (err);
}

/*
 * TC User -> TC Provider Primitives
 * --------------------------------------------------------------------------
 */

static fastcall int
tc_info_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	struct TC_info_req *p;
	mblk_t *mp;
	int err;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!bcanputnext(mc->rq, 0)))
		goto ebusy;
	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = TC_INFO_REQ;
	mp->b_wptr += sizeof(*p);
	freemsg(msg);
	LOGTX(mc, "TC_INFO_REQ ->");
	putnext(mc->rq, mp);
	return (0);
      ebusy:
	err = -EBUSY;
	goto error;
      enobufs:
	err = -ENOBUFS;
	goto error;
      error:
	return (err);
}
static fastcall int
tc_bind_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	struct TC_info_req *p;
	mblk_t *mp;
	int err;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!bcanputnext(mc->rq, 0)))
		goto ebusy;
	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = TC_INFO_REQ;
	mp->b_wptr += sizeof(*p);
	freemsg(msg);
	LOGTX(mc, "TC_INFO_REQ ->");
	putnext(mc->rq, mp);
	return (0);
      ebusy:
	err = -EBUSY;
	goto error;
      enobufs:
	err = -ENOBUFS;
	goto error;
      error:
	return (err);
}
static fastcall int
tc_unbind_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	struct TC_info_req *p;
	mblk_t *mp;
	int err;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!bcanputnext(mc->rq, 0)))
		goto ebusy;
	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = TC_INFO_REQ;
	mp->b_wptr += sizeof(*p);
	freemsg(msg);
	LOGTX(mc, "TC_INFO_REQ ->");
	putnext(mc->rq, mp);
	return (0);
      ebusy:
	err = -EBUSY;
	goto error;
      enobufs:
	err = -ENOBUFS;
	goto error;
      error:
	return (err);
}
static fastcall int
tc_subs_bind_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	struct TC_info_req *p;
	mblk_t *mp;
	int err;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!bcanputnext(mc->rq, 0)))
		goto ebusy;
	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = TC_INFO_REQ;
	mp->b_wptr += sizeof(*p);
	freemsg(msg);
	LOGTX(mc, "TC_INFO_REQ ->");
	putnext(mc->rq, mp);
	return (0);
      ebusy:
	err = -EBUSY;
	goto error;
      enobufs:
	err = -ENOBUFS;
	goto error;
      error:
	return (err);
}
static fastcall int
tc_subs_unbind_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	struct TC_info_req *p;
	mblk_t *mp;
	int err;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!bcanputnext(mc->rq, 0)))
		goto ebusy;
	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = TC_INFO_REQ;
	mp->b_wptr += sizeof(*p);
	freemsg(msg);
	LOGTX(mc, "TC_INFO_REQ ->");
	putnext(mc->rq, mp);
	return (0);
      ebusy:
	err = -EBUSY;
	goto error;
      enobufs:
	err = -ENOBUFS;
	goto error;
      error:
	return (err);
}
static fastcall int
tc_optmgmt_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	struct TC_info_req *p;
	mblk_t *mp;
	int err;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!bcanputnext(mc->rq, 0)))
		goto ebusy;
	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = TC_INFO_REQ;
	mp->b_wptr += sizeof(*p);
	freemsg(msg);
	LOGTX(mc, "TC_INFO_REQ ->");
	putnext(mc->rq, mp);
	return (0);
      ebusy:
	err = -EBUSY;
	goto error;
      enobufs:
	err = -ENOBUFS;
	goto error;
      error:
	return (err);
}
static fastcall int
tc_uni_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	struct TC_info_req *p;
	mblk_t *mp;
	int err;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!bcanputnext(mc->rq, 0)))
		goto ebusy;
	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = TC_INFO_REQ;
	mp->b_wptr += sizeof(*p);
	freemsg(msg);
	LOGTX(mc, "TC_INFO_REQ ->");
	putnext(mc->rq, mp);
	return (0);
      ebusy:
	err = -EBUSY;
	goto error;
      enobufs:
	err = -ENOBUFS;
	goto error;
      error:
	return (err);
}
static fastcall int
tc_begin_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	struct TC_info_req *p;
	mblk_t *mp;
	int err;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!bcanputnext(mc->rq, 0)))
		goto ebusy;
	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = TC_INFO_REQ;
	mp->b_wptr += sizeof(*p);
	freemsg(msg);
	LOGTX(mc, "TC_INFO_REQ ->");
	putnext(mc->rq, mp);
	return (0);
      ebusy:
	err = -EBUSY;
	goto error;
      enobufs:
	err = -ENOBUFS;
	goto error;
      error:
	return (err);
}
static fastcall int
tc_begin_res(struct tc *tc, queue_t *q, mblk_t *mp)
{
	struct TC_info_req *p;
	mblk_t *mp;
	int err;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!bcanputnext(mc->rq, 0)))
		goto ebusy;
	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = TC_INFO_REQ;
	mp->b_wptr += sizeof(*p);
	freemsg(msg);
	LOGTX(mc, "TC_INFO_REQ ->");
	putnext(mc->rq, mp);
	return (0);
      ebusy:
	err = -EBUSY;
	goto error;
      enobufs:
	err = -ENOBUFS;
	goto error;
      error:
	return (err);
}
static fastcall int
tc_cont_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	struct TC_info_req *p;
	mblk_t *mp;
	int err;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!bcanputnext(mc->rq, 0)))
		goto ebusy;
	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = TC_INFO_REQ;
	mp->b_wptr += sizeof(*p);
	freemsg(msg);
	LOGTX(mc, "TC_INFO_REQ ->");
	putnext(mc->rq, mp);
	return (0);
      ebusy:
	err = -EBUSY;
	goto error;
      enobufs:
	err = -ENOBUFS;
	goto error;
      error:
	return (err);
}
static fastcall int
tc_end_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	struct TC_info_req *p;
	mblk_t *mp;
	int err;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!bcanputnext(mc->rq, 0)))
		goto ebusy;
	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = TC_INFO_REQ;
	mp->b_wptr += sizeof(*p);
	freemsg(msg);
	LOGTX(mc, "TC_INFO_REQ ->");
	putnext(mc->rq, mp);
	return (0);
      ebusy:
	err = -EBUSY;
	goto error;
      enobufs:
	err = -ENOBUFS;
	goto error;
      error:
	return (err);
}
static fastcall int
tc_abort_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	struct TC_info_req *p;
	mblk_t *mp;
	int err;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!bcanputnext(mc->rq, 0)))
		goto ebusy;
	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = TC_INFO_REQ;
	mp->b_wptr += sizeof(*p);
	freemsg(msg);
	LOGTX(mc, "TC_INFO_REQ ->");
	putnext(mc->rq, mp);
	return (0);
      ebusy:
	err = -EBUSY;
	goto error;
      enobufs:
	err = -ENOBUFS;
	goto error;
      error:
	return (err);
}
static fastcall int
tc_invoke_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	struct TC_info_req *p;
	mblk_t *mp;
	int err;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!bcanputnext(mc->rq, 0)))
		goto ebusy;
	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = TC_INFO_REQ;
	mp->b_wptr += sizeof(*p);
	freemsg(msg);
	LOGTX(mc, "TC_INFO_REQ ->");
	putnext(mc->rq, mp);
	return (0);
      ebusy:
	err = -EBUSY;
	goto error;
      enobufs:
	err = -ENOBUFS;
	goto error;
      error:
	return (err);
}
static fastcall int
tc_result_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	struct TC_info_req *p;
	mblk_t *mp;
	int err;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!bcanputnext(mc->rq, 0)))
		goto ebusy;
	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = TC_INFO_REQ;
	mp->b_wptr += sizeof(*p);
	freemsg(msg);
	LOGTX(mc, "TC_INFO_REQ ->");
	putnext(mc->rq, mp);
	return (0);
      ebusy:
	err = -EBUSY;
	goto error;
      enobufs:
	err = -ENOBUFS;
	goto error;
      error:
	return (err);
}
static fastcall int
tc_error_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	struct TC_info_req *p;
	mblk_t *mp;
	int err;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!bcanputnext(mc->rq, 0)))
		goto ebusy;
	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = TC_INFO_REQ;
	mp->b_wptr += sizeof(*p);
	freemsg(msg);
	LOGTX(mc, "TC_INFO_REQ ->");
	putnext(mc->rq, mp);
	return (0);
      ebusy:
	err = -EBUSY;
	goto error;
      enobufs:
	err = -ENOBUFS;
	goto error;
      error:
	return (err);
}
static fastcall int
tc_cancel_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	struct TC_info_req *p;
	mblk_t *mp;
	int err;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!bcanputnext(mc->rq, 0)))
		goto ebusy;
	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = TC_INFO_REQ;
	mp->b_wptr += sizeof(*p);
	freemsg(msg);
	LOGTX(mc, "TC_INFO_REQ ->");
	putnext(mc->rq, mp);
	return (0);
      ebusy:
	err = -EBUSY;
	goto error;
      enobufs:
	err = -ENOBUFS;
	goto error;
      error:
	return (err);
}
static fastcall int
tc_reject_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	struct TC_info_req *p;
	mblk_t *mp;
	int err;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!bcanputnext(mc->rq, 0)))
		goto ebusy;
	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = TC_INFO_REQ;
	mp->b_wptr += sizeof(*p);
	freemsg(msg);
	LOGTX(mc, "TC_INFO_REQ ->");
	putnext(mc->rq, mp);
	return (0);
      ebusy:
	err = -EBUSY;
	goto error;
      enobufs:
	err = -ENOBUFS;
	goto error;
      error:
	return (err);
}
static fastcall int
tc_coord_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	struct TC_info_req *p;
	mblk_t *mp;
	int err;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!bcanputnext(mc->rq, 0)))
		goto ebusy;
	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = TC_INFO_REQ;
	mp->b_wptr += sizeof(*p);
	freemsg(msg);
	LOGTX(mc, "TC_INFO_REQ ->");
	putnext(mc->rq, mp);
	return (0);
      ebusy:
	err = -EBUSY;
	goto error;
      enobufs:
	err = -ENOBUFS;
	goto error;
      error:
	return (err);
}
static fastcall int
tc_coord_res(struct tc *tc, queue_t *q, mblk_t *mp)
{
	struct TC_info_req *p;
	mblk_t *mp;
	int err;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!bcanputnext(mc->rq, 0)))
		goto ebusy;
	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = TC_INFO_REQ;
	mp->b_wptr += sizeof(*p);
	freemsg(msg);
	LOGTX(mc, "TC_INFO_REQ ->");
	putnext(mc->rq, mp);
	return (0);
      ebusy:
	err = -EBUSY;
	goto error;
      enobufs:
	err = -ENOBUFS;
	goto error;
      error:
	return (err);
}
static fastcall int
tc_state_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	struct TC_info_req *p;
	mblk_t *mp;
	int err;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!bcanputnext(mc->rq, 0)))
		goto ebusy;
	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = TC_INFO_REQ;
	mp->b_wptr += sizeof(*p);
	freemsg(msg);
	LOGTX(mc, "TC_INFO_REQ ->");
	putnext(mc->rq, mp);
	return (0);
      ebusy:
	err = -EBUSY;
	goto error;
      enobufs:
	err = -ENOBUFS;
	goto error;
      error:
	return (err);
}

/*
 * PROTOCOL STATE MACHINE
 * --------------------------------------------------------------------------
 */

/*
 * MAP User -> MAP Provider Primitives
 * --------------------------------------------------------------------------
 */

static fastcall int
mc_info_req(struct mc *mc, queue_t *q, mblk_t *msg, struct MAP_info_req *p)
{
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	return mc_error_reply(mc, q, msg, p->PRIM_type, err, __FUNCTION__);
}
static fastcall int
mc_bind_req(struct mc *mc, queue_t *q, mblk_t *msg, struct MAP_bind_req *p)
{
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	return mc_error_reply(mc, q, msg, p->PRIM_type, err, __FUNCTION__);
}
static fastcall int
mc_unbind_req(struct mc *mc, queue_t *q, mblk_t *msg, struct MAP_unbind_req *p)
{
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	return mc_error_reply(mc, q, msg, p->PRIM_type, err, __FUNCTION__);
}
static fastcall int
mc_optmgmt_req(struct mc *mc, queue_t *q, mblk_t *msg, struct MAP_optmgmt_req *p)
{
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	return mc_error_reply(mc, q, msg, p->PRIM_type, err, __FUNCTION__);
}
static fastcall int
mc_open_req(struct mc *mc, queue_t *q, mblk_t *msg, struct MAP_open_req *p)
{
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	return mc_error_reply(mc, q, msg, p->PRIM_type, err, __FUNCTION__);
}
static fastcall int
mc_open_res(struct mc *mc, queue_t *q, mblk_t *msg, struct MAP_open_res *p)
{
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	return mc_error_reply(mc, q, msg, p->PRIM_type, err, __FUNCTION__);
}
static fastcall int
mc_refuse_res(struct mc *mc, queue_t *q, mblk_t *msg, struct MAP_refuse_res *p)
{
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	return mc_error_reply(mc, q, msg, p->PRIM_type, err, __FUNCTION__);
}
static fastcall int
mc_serv_req(struct mc *mc, queue_t *q, mblk_t *msg, struct MAP_serv_req *p)
{
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	return mc_error_reply(mc, q, msg, p->PRIM_type, err, __FUNCTION__);
}
static fastcall int
mc_serv_res(struct mc *mc, queue_t *q, mblk_t *msg, struct MAP_serv_res *p)
{
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	return mc_error_reply(mc, q, msg, p->PRIM_type, err, __FUNCTION__);
}
static fastcall int
mc_delim_req(struct mc *mc, queue_t *q, mblk_t *msg, struct MAP_delim_req *p)
{
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	return mc_error_reply(mc, q, msg, p->PRIM_type, err, __FUNCTION__);
}
static fastcall int
mc_close_req(struct mc *mc, queue_t *q, mblk_t *msg, struct MAP_close_req *p)
{
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	return mc_error_reply(mc, q, msg, p->PRIM_type, err, __FUNCTION__);
}
static fastcall int
mc_abort_req(struct mc *mc, queue_t *q, mblk_t *msg, struct MAP_abort_req *p)
{
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	return mc_error_reply(mc, q, msg, p->PRIM_type, err, __FUNCTION__);
}
static fastcall int
mc_stc1_req(struct mc *mc, queue_t *q, mblk_t *msg, struct MAP_stc1_req *p)
{
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	return mc_error_reply(mc, q, msg, p->PRIM_type, err, __FUNCTION__);
}
static fastcall int
mc_stc2_req(struct mc *mc, queue_t *q, mblk_t *msg, struct MAP_stc2_req *p)
{
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	return mc_error_reply(mc, q, msg, p->PRIM_type, err, __FUNCTION__);
}
static fastcall int
mc_stc3_req(struct mc *mc, queue_t *q, mblk_t *msg, struct MAP_stc3_req *p)
{
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	return mc_error_reply(mc, q, msg, p->PRIM_type, err, __FUNCTION__);
}
static fastcall int
mc_stc4_req(struct mc *mc, queue_t *q, mblk_t *msg, struct MAP_stc4_req *p)
{
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	return mc_error_reply(mc, q, msg, p->PRIM_type, err, __FUNCTION__);
}

/*
 * TC Provider -> TC User Primitives
 * --------------------------------------------------------------------------
 */

static fastcall int
tc_info_ack(struct tc *tc, queue_t *q, mblk_t *msg, struct TC_info_ack *p)
{
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
static fastcall int
tc_bind_ack(struct tc *tc, queue_t *q, mblk_t *msg, struct TC_bind_ack *p)
{
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
static fastcall int
tc_subs_bind_ack(struct tc *tc, queue_t *q, mblk_t *msg, struct TC_subs_bind_ack *p)
{
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
static fastcall int
tc_ok_ack(struct tc *tc, queue_t *q, mblk_t *msg, struct TC_ok_ack *p)
{
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
static fastcall int
tc_error_ack(struct tc *tc, queue_t *q, mblk_t *msg, struct TC_error_ack *p)
{
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
static fastcall int
tc_optmgmt_ack(struct tc *tc, queue_t *q, mblk_t *msg, struct TC_optmgmt_ack *p)
{
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * tc_uni_ind: - process TC-UNI Indication from TC
 * @tc: TC private structure
 * @q: active queue (lower read queue)
 * @mp: the primitive
 * @p: a pointer to the primtiive structure
 */
static fastcall int
tc_uni_ind(struct tc *tc, queue_t *q, mblk_t *mp, struct TC_uni_ind *p)
{
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * tc_begin_ind: - process TC-BEGIN Indication from TC
 * @tc: TC private structure
 * @q: active queue (lower read queue)
 * @mp: the primitive
 * @p: a pointer to the primtiive structure
 *
 * The TC_BEGIN_IND primitive maps to the MAP_OPEN_IND primitive of MAP.  The components
 * (when present) map to MAP_SERV_IND and MAP_SERV_CON primitives followed by a
 * MAP_DELIM_IND primitive.  The TC_BEGIN_IND also generates a new MAP dialogue.
 */
static fastcall int
tc_begin_ind(struct tc *tc, queue_t *q, mblk_t *mp, struct TC_begin_ind *p)
{
	int err;
	caddr_t sptr = NULL, dptr = NULL, optr = NULL;
	size_T slen = 0, dlen = 0, olen = 0;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto emsgsize;
	if ((slen = p->SRC_length)) {
		if (slen != tc->info.ADDR_size)
			goto badaddr;
		if (!MBLKIN(mp, p->SRC_offset, p->SRC_length))
			goto badaddr;
		sptr = (caddr_t) p + p->SRC_offset;
	}
	if ((dlen = p->SRC_length)) {
		if (dlen != tc->info.ADDR_size)
			goto badaddr;
		if (!MBLKIN(mp, p->SRC_offset, p->SRC_length))
			goto badaddr;
		dptr = (caddr_t) p + p->SRC_offset;
	}
	if ((olen = p->OPT_length)) {
		if (olen > tc->info.OPT_size)
			goto badopt;
		if (!MBLKIN(mp, p->OPT_offset, p->OPT_length))
			goto badopt;
		optr = (caddr_t) p + p->OPT_offset;
	}
      badaddr:
	err = TCBADADDR;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * tc_begin_con: - process TC-CONTINUE Indication (confirming) from TC
 * @tc: TC private structure
 * @q: active queue (lower read queue)
 * @mp: the primitive
 * @p: a pointer to the primtiive structure
 */
static fastcall int
tc_begin_con(struct tc *tc, queue_t *q, mblk_t *mp, struct TC_begin_con *p)
{
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * tc_cont_ind: - process TC-CONTINUE Indication (continuing) from TC
 * @tc: TC private structure
 * @q: active queue (lower read queue)
 * @mp: the primitive
 * @p: a pointer to the primtiive structure
 */
static fastcall int
tc_cont_ind(struct tc *tc, queue_t *q, mblk_t *mp, struct TC_cont_ind *p)
{
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * tc_end_ind: - process TC-END Indication from TC
 * @tc: TC private structure
 * @q: active queue (lower read queue)
 * @mp: the primitive
 * @p: a pointer to the primtiive structure
 */
static fastcall int
tc_end_ind(struct tc *tc, queue_t *q, mblk_t *mp, struct TC_end_ind *p)
{
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * tc_abort_ind: - process TC-ABORT Indication from TC
 * @tc: TC private structure
 * @q: active queue (lower read queue)
 * @mp: the primitive
 * @p: a pointer to the primtiive structure
 */
static fastcall int
tc_abort_ind(struct tc *tc, queue_t *q, mblk_t *mp, struct TC_abort_ind *p)
{
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * tc_noticed_ind: - process TC-NOTICE Indication from TC
 * @tc: TC private structure
 * @q: active queue (lower read queue)
 * @mp: the primitive
 * @p: a pointer to the primtiive structure
 */
static fastcall int
tc_notice_ind(struct tc *tc, queue_t *q, mblk_t *mp, struct TC_notice_ind *p)
{
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * tc_invoke_ind: - process TC-INVOKE Indication from TC
 * @tc: TC private structure
 * @q: active queue (lower read queue)
 * @mp: the primitive
 * @p: a pointer to the primtiive structure
 */
static fastcall int
tc_invoke_ind(struct tc *tc, queue_t *q, mblk_t *mp, struct TC_invoke_ind *p)
{
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * tc_result_ind: - process TC-RESULT Indication from TC
 * @tc: TC private structure
 * @q: active queue (lower read queue)
 * @mp: the primitive
 * @p: a pointer to the primtiive structure
 */
static fastcall int
tc_result_ind(struct tc *tc, queue_t *q, mblk_t *mp, struct TC_result_ind *p)
{
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * tc_error_ind: - process TC-ERROR Indication from TC
 * @tc: TC private structure
 * @q: active queue (lower read queue)
 * @mp: the primitive
 * @p: a pointer to the primtiive structure
 */
static fastcall int
tc_error_ind(struct tc *tc, queue_t *q, mblk_t *mp, struct TC_error_ind *p)
{
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * tc_cancel_ind: - process TC-CANCEL Indication from TC
 * @tc: TC private structure
 * @q: active queue (lower read queue)
 * @mp: the primitive
 * @p: a pointer to the primtiive structure
 */
static fastcall int
tc_cancel_ind(struct tc *tc, queue_t *q, mblk_t *mp, struct TC_cancel_ind *p)
{
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * tc_reject_ind: - process TC-REJECT Indication from TC
 * @tc: TC private structure
 * @q: active queue (lower read queue)
 * @mp: the primitive
 * @p: a pointer to the primtiive structure
 */
static fastcall int
tc_reject_ind(struct tc *tc, queue_t *q, mblk_t *mp, struct TC_reject_ind *p)
{
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
static fastcall int
tc_coord_ind(struct tc *tc, queue_t *q, mblk_t *mp, struct TC_coord_ind *p)
{
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
static fastcall int
tc_coord_con(struct tc *tc, queue_t *q, mblk_t *mp, struct TC_coord_con *p)
{
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
static fastcall int
tc_state_ind(struct tc *tc, queue_t *q, mblk_t *mp, struct TC_state_ind *p)
{
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
static fastcall int
tc_pcstate_ind(struct tc *tc, queue_t *q, mblk_t *mp, struct TC_pcstate_ind *p)
{
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
static fastcall int
tc_traffic_ind(struct tc *tc, queue_t *q, mblk_t *mp, struct TC_traffic_ind *p)
{
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * tc_other_ind: - process unknown TC indication or confirmation from TC
 * @tc: TC private structure
 * @q: active queue (lower read queue)
 * @mp: the primitive
 * @p: a pointer to the primtiive structure
 */
static fastcall int
tc_other_ind(struct tc *tc, queue_t *q, mblk_t *mp, t_uscalar_t *p)
{
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/*
 * INPUT-OUTPUT CONTROLS
 * --------------------------------------------------------------------------
 */

static void
mc_copy_done(struct mc *mc, queue_t *q, mblk_t *mp, int err)
{
	if (err == 0) {
		LOGIO(mc, "<- M_IOCACK(%s)", mc_iocname(((struct iocblk *) mp->b_rptr)->ioc_cmd));
	} else {
		LOGIO(mc, "<- M_IOCNAK(%s)", mc_iocname(((struct iocblk *) mp->b_rptr)->ioc_cmd));
	}
	mi_copy_done(q, mp, err);
}
static void
mc_copyin(struct mc *mc, queue_t *q, mblk_t *mp, caddr_t uaddr, size_t len)
{
	LOGIO(mc, "<- M_COPYIN(%s)", mc_iocname(((struct iocblk *) mp->b_rptr)->ioc_cmd));

	mi_copyin(q, mp, uaddr, len);
}
static void
mc_copyin_n(struct mc *mc, queue_t *q, mblk_t *mp, caddr_t offset, size_t len)
{
	LOGIO(mc, "<- M_COPYIN(%s)", mc_iocname(((struct iocblk *) mp->b_rptr)->ioc_cmd));

	mi_copyin_n(q, mp, offset, len);
}
static void
mc_copyout(struct mc *mc, queue_t *q, mblk_t *mp)
{
	LOGIO(mc, "<- M_COPYOUT(%s)", mc_iocname(((struct iocblk *) mp->b_rptr)->ioc_cmd));

	mi_copyout(q, mp);
}

/*
 * INPUT-OUTPUT CONTROL HANDLING
 * --------------------------------------------------------------------------
 */
/**
 * mc_i_link: - process an I_LINK M_IOCTL message
 * @lm: MAP private structure
 * @q: active queue (upper write queue)
 * @mp: the I_LINK M_IOCTL message
 *
 * This driver supports I_LINK operations; however, lower TC streams that are linked
 * using the I_LINK operation can only be used by the upper MAP stream that performed
 * the linking operation (the "management" stream).  This has the effect of attaching
 * the upper MAP stream to the lower TC stream, even though a bind has not yet been
 * performed.  When the upper MAP stream is already attached to a lower TC stream,
 * the I_LINK operation will fail.  (This is because it is not possible for the
 * "management" stream to use more than one linked stream at a time.)
 */
static noinline fastcall __unlikely int
mc_i_link(struct mc *mc, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct linkblk *l = (typeof(l)) mp->b_cont->b_rptr;
	psw_t flags;
	struct mc *mc;
	union {
		struct tc *tc;
		caddr_t ptr;
	} priv = NULL;
	struct tc *tc = NULL;
	int err;
	struct TC_info_req *p;
	mblk_t *rp = NULL;
	dev_t dev = 0;

	if (!(rp = mi_allocb(q, sizeof(*p), BPRI_MED)))
		goto enobufs;
	if (!(priv.ptr = mi_open_alloc_cache(tc_priv_cachep, GFP_ATOMIC)))
		goto enomem;
	tc_init(tc, mc, l->l_qtop, l->l_index);

	write_lock_irqsave(&mc_lock, flags);
	if ((err = mi_open_link(&mc->tc_links, priv.ptr, &dev, 0, MODOPEN, ioc->ioc_cr)))
		goto unlock_error;
	tc->mgr = mc;
	mi_attach(l->l_qtop, priv.ptr);
	write_unlock_irqrestore(&mc_lock, flags);

	/* immediate info request */
	DB_TYPE(rp) = M_PCPROTO;
	p = (typeof(p)) rp->b_wptr;
	p->PRIM_type = TC_INFO_REQ;
	rp->b_wptr += sizeof(*p);
	putnext(tc->wq, rp);

	mc_copy_done(mc, q, mp, 0);
	return (0);
      enomem:
	err = ENOMEM;
	goto error;
      enobufs:
	err = ENOBUFS;
	goto error;
      unlock_error:
	write_unlock_irqrestore(&mc_lock, flags);
	goto error;
      error:
	if (rp)
		freeb(rp);
	if (tc)
		mi_close_free_cache(tc_priv_cachep, (caddr_t) tc);
	return (-err);
}

/**
 * mc_i_plink: - process an I_PLINK M_IOCTL message
 * @mc: MAP private structure
 * @q: active queue (upper write queue)
 * @mp: the I_PLINK M_IOCTL message
 *
 * This driver supports I_PLINK operations primarily.  Lower TC streams linked using
 * the I_PLINK operation are available to be used by any upper MAP stream.  It is
 * questionable whether we should allow other than a privileged user to link a lower
 * Stream.
 */
static noinline fastcall __unlikely int
mc_i_plink(struct mc *mc, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct linkblk *l = (typeof(l)) mp->b_cont->b_rptr;
	psw_t flags;
	union {
		struct tc *tc;
		caddr_t ptr;
	} priv = NULL;
	int err;
	struct TC_info_req *p;
	mblk_t *rp = NULL;
	dev_t dev = 0;

	if (!(rp = mi_allocb(q, sizeof(*p), BPRI_MED)))
		goto enobufs;
	if (!(priv.ptr = mi_open_alloc_cache(tc_priv_cachep, GFP_ATOMIC)))
		goto enomem;
	tc_init(priv.tc, mc, l->l_qtop, l->l_index);

	write_lock_irqsave(&mc_lock, flags);
	if ((err = mi_open_link(&tc_links, priv.ptr, &dev, 0, MODOPEN, ioc->ioc_cr)))
		goto unlock_error;
	mi_attach(l->l_qtop, priv.ptr);
	tc->mgr = NULL;
	write_unlock_irqrestore(&mc_lock, flags);

	/* immediate info request */
	DB_TYPE(rp) = M_PCPROTO;
	p = (typeof(p)) rp->b_wptr;
	p->PRIM_type = TC_INFO_REQ;
	rp->b_wptr += sizeof(*p);
	putnext(tc->wq, rp);

	mc_copy_done(mc, q, mp, 0);
	return (0);
      enobufs:
	err = ENOBUFS;
	goto error;
      enomem:
	err = ENOMEM;
	goto error;
      unlock_error:
	write_unlock_irqrestore(&mc_lock, flags);
	goto error;
      error:
	if (rp)
		freeb(rp);
	if (priv.ptr)
		mi_close_free_cache(tc_priv_cachep, priv.ptr);
	return (-err);
}

/**
 * mc_i_unlink: - process an I_UNLINK M_IOCTL message
 * @mc: MAP private structure
 * @q: active queue (upper write queue)
 * @mp: the I_UNLINK M_IOCTL message
 */
static noinline fastcall __unlikely int
mc_i_unlink(struct mc *mc, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct linkblk *l = (typeof(l)) mp->b_cont->b_rptr;
	union {
		struct tc *tc;
		caddr_t ptr;
	} priv;
	psw_t flags;
	int err;

	write_lock_irqsave(&mc_lock, flags);
	if (!(priv.tc = TC_PRIV(l->l_qtop)))
		goto einval;
	if (!(priv.ptr = mi_acquire(priv.ptr, q)))
		goto edeadlk;
	/* XXX: We might need to abort any dialogues associated with the unlinking TC, or it is
	   possible that we just mark the TC as disconnected and wait for timers to fire or the MAP 
	   user to attempt to communicate with the peer and cancel or abort at that time. */
	mi_close_unlink(&mc->tc_links, priv.ptr);
	mi_detach(l->l_qtop, priv.ptr);
	mi_release(priv.ptr);
	write_unlock_irqrestore(&mc_lock, flags);
	mi_close_free_cachep(tc_priv_cachep, priv.ptr);
	mc_copy_done(mc, q, mp, 0);
	return (0);
      einval:
	err = EINVAL;
	goto error;
      edeadlk:
	err = EDEADLK;
	goto error;
      error:
	write_unlock_irqrestore(&mc_lock, flags);
	return (-err);
}

/**
 * mc_i_punlink: - process an I_PUNLINK M_IOCTL message
 * @mc: MAP private structure
 * @q: active queue (upper write queue)
 * @mp: the I_PUNLINK M_IOCTL message
 */
static noinline fastcall __unlikely int
mc_i_punlink(struct mc *mc, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct linkblk *l = (typeof(l)) mp->b_cont->b_rptr;
	psw_t flags;
	union {
		struct tc *tc;
		caddr_t ptr;
	} priv;
	int err;

	write_lock_irqsave(&mc_lock, flags);
	if (!(tc = TC_PRIV(l->l_qtop)))
		goto einval;
	/* Always issued by user, check credentials.  Only the master control Stream is allowed to
	   create permanent links, however, to avoid difficulties with hanging permanent links,
	   permit the owner of the link or the super user to undo permanent links. */
	if (mc != mc_ctrl && drv_priv(ioc->ioc_cr) != 0 && ioc->ioc_cr->cr_uid != tc->cred.cr_uid)
		goto eperm;
	if (!(priv.ptr = mi_acquire(priv.ptr, q)))
		goto edeadlk;
	/* XXX: We might need to abort any dialogues associated with the unlinking TC, or it is
	   possible that we just mark the TC as disconnected and wait for timers to fire or the MAP 
	   user to attempt to communicate with the perr and cance or abort at that time. */
	mi_close_unlink(&tc_links, priv.ptr);
	mi_detach(l->l_qtop, priv.ptr);
	write_unlock_irqrestore(&mc_lock, flags);
	mi_close_free_cachep(tc_priv_cachep, priv.ptr);
	mc_copy_done(mc, q, mp, 0);
	return (0);
      einval:
	err = EINVAL;
	goto error;
      eperm:
	err = EPERM;
	goto error;
      edeadlk:
	err = EDEADLK;
	goto error;
      esrch:
	err = ESRCH;
	goto error;
      error:
	write_unlock_irqrestore(&mc_lock, flags);
	return (-err);
}

/*
 * MAP INPUT-OUTPUT CONTROLS
 * --------------------------------------------------------------------------
 */
static int
mc_map_ioctl(struct mc *mc, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	int size = -1;

	switch (_IOC_NR(ioc->ioc_cmd)) {
	case _IOC_NR(MAP_IOCGOPTION):
	case _IOC_NR(MAP_IOCSOPTION):
		size = sizeof(struct mc_option);
		break;
	case _IOC_NR(MAP_IOCSCONFIG):
	case _IOC_NR(MAP_IOCGCONFIG):
	case _IOC_NR(MAP_IOCTCONFIG):
	case _IOC_NR(MAP_IOCCCONFIG):
	case _IOC_NR(MAP_IOCLCONFIG):
		size = sizeof(struct mc_config);
		break;
	case _IOC_NR(MAP_IOCGSTATEM):
	case _IOC_NR(MAP_IOCCMRESET):
		size = sizeof(struct mc_statem);
		break;
	case _IOC_NR(MAP_IOCGSTATSP):
	case _IOC_NR(MAP_IOCSSTATSP):
	case _IOC_NR(MAP_IOCGSTATS):
	case _IOC_NR(MAP_IOCCSTATS):
		size = sizeof(struct mc_stats);
		break;
	case _IOC_NR(MAP_IOCGNOTIFY):
	case _IOC_NR(MAP_IOCSNOTIFY):
	case _IOC_NR(MAP_IOCCNOTIFY):
		size = sizeof(struct mc_notify);
		break;
	case _IOC_NR(MAP_IOCCMGMT):
		size = sizeof(struct mc_mgmt);
		break;
	case _IOC_NR(MAP_IOCCPASS):
		size = sizeof(struct mc_pass);
		break;
	default:
		mc_copy_done(mc, q, mp, EOPNOTSUPP);
		return (0);
	}
	if (size == -1)
		mc_copyout(mc, q, mp);
	else
		mc_copyin(mc, q, mp, NULL, size);
	return (0);
}
static int
mc_map_copyin(struct mc *mc, queue_t *q, mblk_t *mp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	mblk_t *bp = NULL, *dp = mp->b_cont;
	int size = -1;
	int err = 0;

	switch (_IOC_NR(cp->cp_cmd)) {
		int len;

	case _IOC_NR(MAP_IOCGOPTION):
	case _IOC_NR(MAP_IOCGCONFIG):
	case _IOC_NR(MAP_IOCGSTATEM):
	case _IOC_NR(MAP_IOCGSTATSP):
	case _IOC_NR(MAP_IOCGSTATS):
	case _IOC_NR(MAP_IOCCSTATS):
		len = sizeof(union mc_ioctls);
		if (!(bp = mi_copyout_alloc(q, mp, NULL, len, false)))
			goto enobufs;
		bcopy(dp->b_rptr, bp->b_rptr, min(dp->b_wptr - dp->b_rptr, len));
		break;
	case _IOC_NR(MAP_IOCLCONFIG):
	{
		struct mc_config *p = (typeof(p)) dp->b_rptr;

		len = (p->cmd + 1) * sizeof(*p);
		if (!(bp = mi_copyout_alloc(q, mp, NULL, len, false)))
			goto enobufs;
		bcopy(dp->b_rptr, bp->b_rptr, min(dp->b_wptr - dp->b_rptr, len));
		break;
	}
	}
	switch (_IOC_NR(cp->cp_cmd)) {
	case _IOC_NR(MAP_IOCGOPTION):
		if (bp == NULL)
			goto enobufs;
		err = mc_get_options(bp);
		break;
	case _IOC_NR(MAP_IOCSOPTION):
		err = mc_size_opt_conf(dp->b_rptr, &size);
		break;
	case _IOC_NR(MAP_IOCGCONFIG):
		if (bp == NULL)
			goto enobufs;
		err = mc_get_conf(bp);
		break;
	case _IOC_NR(MAP_IOCSCONFIG):
	case _IOC_NR(MAP_IOCTCONFIG):
	case _IOC_NR(MAP_IOCCCONFIG):
		err = mc_size_conf(dp->b_rptr, &size);
		break;
	case _IOC_NR(MAP_IOCLCONFIG):
		if (bp == NULL)
			goto enobufs;
		err = mc_lst_conf(bp);
		break;
	case _IOC_NR(MAP_IOCGSTATEM):
		if (bp == NULL)
			goto enobufs;
		err = mc_get_statem(bp);
		break;
	case _IOC_NR(MAP_IOCCMRESET):
		err = EOPNOTSUPP;	/* later */
		break;
	case _IOC_NR(MAP_IOCGSTATSP):
		if (bp == NULL)
			goto enobufs;
		err = mc_do_statsp(bp, MAP_GET);
		break;
	case _IOC_NR(MAP_IOCSSTATSP):
		err = mc_size_stats(dp->b_rptr, &size);
		break;
	case _IOC_NR(MAP_IOCGSTATS):
		if (bp == NULL)
			goto enobufs;
		err = mc_get_stats(bp, false);
		break;
	case _IOC_NR(MAP_IOCCSTATS):
		if (bp == NULL)
			goto enobufs;
		err = mc_get_stats(bp, true);
		break;
	case _IOC_NR(MAP_IOCGNOTIFY):
		if (bp == NULL)
			goto enobufs;
		err = mc_do_notify(bp, 0, MAP_GET);
		break;
	case _IOC_NR(MAP_IOCSNOTIFY):
	case _IOC_NR(MAP_IOCCNOTIFY):
		err = mc_size_notify(dp->b_rptr, &size);
		break;
	case _IOC_NR(MAP_IOCCMGMT):
		err = mc_action(q, dp);
		break;
	case _IOC_NR(MAP_IOCCPASS):
		break;
	default:
		err = EOPNOTSUPP;
		break;
	      enobufs:
		err = ENOBUFS;
		break;
	}
      done:
	if (err < 0)
		return (err);
	if (err > 0) {
		mc_copy_done(mc, q, mp, err);
		return (0);
	}
	if (size == -1)
		mi_copyout(mc, q, mp);
	else
		mi_copyin_n(mc, q, mp, 0, size);
	return (0);
}
static int
mc_map_copyin2(struct mc *mc, queue_t *q, mblk_t *mp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	int err = 0;
	mblk_t *bp;

	if (!(bp = mi_copyout_alloc(q, mp, NULL, dp->b_wptr - dp->b_rptr, false)))
		goto enobufs;
	bcopy(dp->b_rptr, bp->b_rptr, dp->b_wptr - dp->b_rptr);

	switch (_IOC_NR(cp->cp_cmd)) {
	case _IOC_NR(MAP_IOCGOPTION):
		err = EPROTO;
		break;
	case _IOC_NR(MAP_IOCSOPTION):
		err = mc_set_options(bp);
		break;
	case _IOC_NR(MAP_IOCGCONFIG):
		err = EPROTO;
		break;
	case _IOC_NR(MAP_IOCSCONFIG):
		err = mc_set_conf(bp);
		break;
	case _IOC_NR(MAP_IOCTCONFIG):
		err = mc_test_conf(bp);
		break;
	case _IOC_NR(MAP_IOCCCONFIG):
		err = mc_commit_conf(bp);
		break;
#ifdef MAP_IOCLCONFIG
	case _IOC_NR(MAP_IOCLCONFIG):
		err = EPROTO;
		break;
#endif				/* MAP_IOCLCONFIG */
	case _IOC_NR(MAP_IOCGSTATEM):
		err = EPROTO;
		break;
	case _IOC_NR(MAP_IOCCMRESET):
		err = EPROTO;
		break;
	case _IOC_NR(MAP_IOCGSTATSP):
		err = EPROTO;
		break;
	case _IOC_NR(MAP_IOCSSTATSP):
		err = mc_do_statsp(bp, MAP_CHA);
		break;
	case _IOC_NR(MAP_IOCGSTATS):
		err = EPROTO;
		break;
	case _IOC_NR(MAP_IOCCSTATS):
		err = EPROTO;
		break;
	case _IOC_NR(MAP_IOCGNOTIFY):
		err = EPROTO;
		break;
	case _IOC_NR(MAP_IOCSNOTIFY):
		err = mc_do_notify(bp, 0, MAP_ADD);
		break;
	case _IOC_NR(MAP_IOCCNOTIFY):
		err = mc_do_notify(bp, 0, MAP_DEL);
		break;
	case _IOC_NR(MAP_IOCCMGMT):
		err = EPROTO;
		break;
	case _IOC_NR(MAP_IOCCPASS):
	{
		struct mc_pass *p = (typeof(p)) dp->b_rptr;

		break;
	}
	default:
		err = EOPNOTSUPP;
		break;
	}
      done:
	if (err < 0)
		return (err);
	if (err > 0)
		mc_copy_done(mc, q, mp, err);
	if (err == 0)
		mc_copyout(mc, q, mp);
	return (0);
      enobufs:
	err = ENOBUFS;
	goto done;
}
static int
mc_map_copyout(struct mc *mc, queue_t *q, mblk_t *mp)
{
	mc_copyout(mc, q, mp);
	return (0);
}

/*
 * TCAP INPUT-OUTPUT CONTROLS
 * --------------------------------------------------------------------------
 */
static int
mc_tcap_ioctl(struct mc *mc, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	int size = -1;

	switch (_IOC_NR(ioc->ioc_cmd)) {
	case _IOC_NR(TCAP_IOCGOPTION):
	case _IOC_NR(TCAP_IOCSOPTION):
		size = sizeof(struct tcap_option);
		break;
	case _IOC_NR(TCAP_IOCSCONFIG):
	case _IOC_NR(TCAP_IOCGCONFIG):
	case _IOC_NR(TCAP_IOCTCONFIG):
	case _IOC_NR(TCAP_IOCCCONFIG):
	case _IOC_NR(TCAP_IOCLCONFIG):
		size = sizeof(struct tcap_config);
		break;
	case _IOC_NR(TCAP_IOCGSTATEM):
	case _IOC_NR(TCAP_IOCCMRESET):
		size = sizeof(struct tcap_statem);
		break;
	case _IOC_NR(TCAP_IOCGSTATSP):
	case _IOC_NR(TCAP_IOCSSTATSP):
	case _IOC_NR(TCAP_IOCGSTATS):
	case _IOC_NR(TCAP_IOCCSTATS):
		size = sizeof(struct tcap_stats);
		break;
	case _IOC_NR(TCAP_IOCGNOTIFY):
	case _IOC_NR(TCAP_IOCSNOTIFY):
	case _IOC_NR(TCAP_IOCCNOTIFY):
		size = sizeof(struct tcap_notify);
		break;
	case _IOC_NR(TCAP_IOCCMGMT):
		size = sizeof(struct tcap_mgmt);
		break;
	case _IOC_NR(TCAP_IOCCPASS):
		size = sizeof(struct tcap_pass);
		break;
	default:
		mc_copy_done(mc, q, mp, EOPNOTSUPP);
		return (0);
	}
	if (size == -1)
		mc_copyout(mc, q, mp);
	else
		mc_copyin(mc, q, mp, NULL, size);
	return (0);
}
static int
mc_tcap_copyin(struct mc *mc, queue_t *q, mblk_t *mp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	mblk_t *bp = NULL, *dp = mp->b_cont;
	int size = -1;
	int err = 0;

	switch (_IOC_NR(cp->cp_cmd)) {
		int len;

	case _IOC_NR(TCAP_IOCGOPTION):
	case _IOC_NR(TCAP_IOCGCONFIG):
	case _IOC_NR(TCAP_IOCGSTATEM):
	case _IOC_NR(TCAP_IOCGSTATSP):
	case _IOC_NR(TCAP_IOCGSTATS):
	case _IOC_NR(TCAP_IOCCSTATS):
		len = sizeof(union tcap_ioctls);
		if (!(bp = mi_copyout_alloc(q, mp, NULL, len, false)))
			goto enobufs;
		bcopy(dp->b_rptr, bp->b_rptr, min(dp->b_wptr - dp->b_rptr, len));
		break;
	case _IOC_NR(TCAP_IOCLCONFIG):
	{
		struct tcap_config *p = (typeof(p)) dp->b_rptr;

		len = (p->cmd + 1) * sizeof(*p);
		if (!(bp = mi_copyout_alloc(q, mp, NULL, len, false)))
			goto enobufs;
		bcopy(dp->b_rptr, bp->b_rptr, min(dp->b_wptr - dp->b_rptr, len));
		break;
	}
	}
	switch (_IOC_NR(cp->cp_cmd)) {
	case _IOC_NR(TCAP_IOCGOPTION):
		if (bp == NULL)
			goto enobufs;
		err = tcap_get_options(bp);
		break;
	case _IOC_NR(TCAP_IOCSOPTION):
		err = tcap_size_opt_conf(dp->b_rptr, &size);
		break;
	case _IOC_NR(TCAP_IOCGCONFIG):
		if (bp == NULL)
			goto enobufs;
		err = tcap_get_conf(bp);
		break;
	case _IOC_NR(TCAP_IOCSCONFIG):
	case _IOC_NR(TCAP_IOCTCONFIG):
	case _IOC_NR(TCAP_IOCCCONFIG):
		err = tcap_size_conf(dp->b_rptr, &size);
		break;
	case _IOC_NR(TCAP_IOCLCONFIG):
		if (bp == NULL)
			goto enobufs;
		err = tcap_lst_conf(bp);
		break;
	case _IOC_NR(TCAP_IOCGSTATEM):
		if (bp == NULL)
			goto enobufs;
		err = tcap_get_statem(bp);
		break;
	case _IOC_NR(TCAP_IOCCMRESET):
		err = EOPNOTSUPP;	/* later */
		break;
	case _IOC_NR(TCAP_IOCGSTATSP):
		if (bp == NULL)
			goto enobufs;
		err = tcap_do_statsp(bp, TCAP_GET);
		break;
	case _IOC_NR(TCAP_IOCSSTATSP):
		err = tcap_size_stats(dp->b_rptr, &size);
		break;
	case _IOC_NR(TCAP_IOCGSTATS):
		if (bp == NULL)
			goto enobufs;
		err = tcap_get_stats(bp, false);
		break;
	case _IOC_NR(TCAP_IOCCSTATS):
		if (bp == NULL)
			goto enobufs;
		err = tcap_get_stats(bp, true);
		break;
	case _IOC_NR(TCAP_IOCGNOTIFY):
		if (bp == NULL)
			goto enobufs;
		err = tcap_do_notify(bp, 0, TCAP_GET);
		break;
	case _IOC_NR(TCAP_IOCSNOTIFY):
	case _IOC_NR(TCAP_IOCCNOTIFY):
		err = tcap_size_notify(dp->b_rptr, &size);
		break;
	case _IOC_NR(TCAP_IOCCMGMT):
		err = tcap_action(q, dp);
		break;
	case _IOC_NR(TCAP_IOCCPASS):
		break;
	default:
		err = EOPNOTSUPP;
		break;
	      enobufs:
		err = ENOBUFS;
		break;
	}
      done:
	if (err < 0)
		return (err);
	if (err > 0) {
		mc_copy_done(mc, q, mp, err);
		return (0);
	}
	if (size == -1)
		mi_copyout(mc, q, mp);
	else
		mi_copyin_n(mc, q, mp, 0, size);
	return (0);
}
static int
mc_tcap_copyin2(struct mc *mc, queue_t *q, mblk_t *mp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	int err = 0;
	mblk_t *bp;

	if (!(bp = mi_copyout_alloc(q, mp, NULL, dp->b_wptr - dp->b_rptr, false)))
		goto enobufs;
	bcopy(dp->b_rptr, bp->b_rptr, dp->b_wptr - dp->b_rptr);

	switch (_IOC_NR(cp->cp_cmd)) {
	case _IOC_NR(TCAP_IOCGOPTION):
		err = EPROTO;
		break;
	case _IOC_NR(TCAP_IOCSOPTION):
		err = tcap_set_options(bp);
		break;
	case _IOC_NR(TCAP_IOCGCONFIG):
		err = EPROTO;
		break;
	case _IOC_NR(TCAP_IOCSCONFIG):
		err = tcap_set_conf(bp);
		break;
	case _IOC_NR(TCAP_IOCTCONFIG):
		err = tcap_test_conf(bp);
		break;
	case _IOC_NR(TCAP_IOCCCONFIG):
		err = tcap_commit_conf(bp);
		break;
#ifdef TCAP_IOCLCONFIG
	case _IOC_NR(TCAP_IOCLCONFIG):
		err = EPROTO;
		break;
#endif				/* TCAP_IOCLCONFIG */
	case _IOC_NR(TCAP_IOCGSTATEM):
		err = EPROTO;
		break;
	case _IOC_NR(TCAP_IOCCMRESET):
		err = EPROTO;
		break;
	case _IOC_NR(TCAP_IOCGSTATSP):
		err = EPROTO;
		break;
	case _IOC_NR(TCAP_IOCSSTATSP):
		err = tcap_do_statsp(bp, TCAP_CHA);
		break;
	case _IOC_NR(TCAP_IOCGSTATS):
		err = EPROTO;
		break;
	case _IOC_NR(TCAP_IOCCSTATS):
		err = EPROTO;
		break;
	case _IOC_NR(TCAP_IOCGNOTIFY):
		err = EPROTO;
		break;
	case _IOC_NR(TCAP_IOCSNOTIFY):
		err = tcap_do_notify(bp, 0, TCAP_ADD);
		break;
	case _IOC_NR(TCAP_IOCCNOTIFY):
		err = tcap_do_notify(bp, 0, TCAP_DEL);
		break;
	case _IOC_NR(TCAP_IOCCMGMT):
		err = EPROTO;
		break;
	case _IOC_NR(TCAP_IOCCPASS):
	{
		struct tcap_pass *p = (typeof(p)) dp->b_rptr;

		break;
	}
	default:
		err = EOPNOTSUPP;
		break;
	}
      done:
	if (err < 0)
		return (err);
	if (err > 0)
		mc_copy_done(mc, q, mp, err);
	if (err == 0)
		mc_copyout(mc, q, mp);
	return (0);
      enobufs:
	err = ENOBUFS;
	goto done;
}
static int
mc_tcap_copyout(struct mc *mc, queue_t *q, mblk_t *mp)
{
	mc_copyout(mc, q, mp);
	return (0);
}

/*
 * SCCP INPUT-OUTPUT CONTROLS
 * --------------------------------------------------------------------------
 */
static int
mc_sccp_ioctl(struct mc *mc, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	int size = -1;

	switch (_IOC_NR(ioc->ioc_cmd)) {
	case _IOC_NR(SCCP_IOCGOPTION):
	case _IOC_NR(SCCP_IOCSOPTION):
		size = sizeof(struct sccp_option);
		break;
	case _IOC_NR(SCCP_IOCSCONFIG):
	case _IOC_NR(SCCP_IOCGCONFIG):
	case _IOC_NR(SCCP_IOCTCONFIG):
	case _IOC_NR(SCCP_IOCCCONFIG):
	case _IOC_NR(SCCP_IOCLCONFIG):
		size = sizeof(struct sccp_config);
		break;
	case _IOC_NR(SCCP_IOCGSTATEM):
	case _IOC_NR(SCCP_IOCCMRESET):
		size = sizeof(struct sccp_statem);
		break;
	case _IOC_NR(SCCP_IOCGSTATSP):
	case _IOC_NR(SCCP_IOCSSTATSP):
	case _IOC_NR(SCCP_IOCGSTATS):
	case _IOC_NR(SCCP_IOCCSTATS):
		size = sizeof(struct sccp_stats);
		break;
	case _IOC_NR(SCCP_IOCGNOTIFY):
	case _IOC_NR(SCCP_IOCSNOTIFY):
	case _IOC_NR(SCCP_IOCCNOTIFY):
		size = sizeof(struct sccp_notify);
		break;
	case _IOC_NR(SCCP_IOCCMGMT):
		size = sizeof(struct sccp_mgmt);
		break;
	case _IOC_NR(SCCP_IOCCPASS):
		size = sizeof(struct sccp_pass);
		break;
	default:
		mc_copy_done(mc, q, mp, EOPNOTSUPP);
		return (0);
	}
	if (size == -1)
		mc_copyout(mc, q, mp);
	else
		mc_copyin(mc, q, mp, NULL, size);
	return (0);
}
static int
mc_sccp_copyin(struct mc *mc, queue_t *q, mblk_t *mp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	mblk_t *bp = NULL, *dp = mp->b_cont;
	int size = -1;
	int err = 0;

	switch (_IOC_NR(cp->cp_cmd)) {
		int len;

	case _IOC_NR(SCCP_IOCGOPTION):
	case _IOC_NR(SCCP_IOCGCONFIG):
	case _IOC_NR(SCCP_IOCGSTATEM):
	case _IOC_NR(SCCP_IOCGSTATSP):
	case _IOC_NR(SCCP_IOCGSTATS):
	case _IOC_NR(SCCP_IOCCSTATS):
		len = sizeof(union sccp_ioctls);
		if (!(bp = mi_copyout_alloc(q, mp, NULL, len, false)))
			goto enobufs;
		bcopy(dp->b_rptr, bp->b_rptr, min(dp->b_wptr - dp->b_rptr, len));
		break;
	case _IOC_NR(SCCP_IOCLCONFIG):
	{
		struct sccp_config *p = (typeof(p)) dp->b_rptr;

		len = (p->cmd + 1) * sizeof(*p);
		if (!(bp = mi_copyout_alloc(q, mp, NULL, len, false)))
			goto enobufs;
		bcopy(dp->b_rptr, bp->b_rptr, min(dp->b_wptr - dp->b_rptr, len));
		break;
	}
	}
	switch (_IOC_NR(cp->cp_cmd)) {
	case _IOC_NR(SCCP_IOCGOPTION):
		if (bp == NULL)
			goto enobufs;
		err = sccp_get_options(bp);
		break;
	case _IOC_NR(SCCP_IOCSOPTION):
		err = sccp_size_opt_conf(dp->b_rptr, &size);
		break;
	case _IOC_NR(SCCP_IOCGCONFIG):
		if (bp == NULL)
			goto enobufs;
		err = sccp_get_conf(bp);
		break;
	case _IOC_NR(SCCP_IOCSCONFIG):
	case _IOC_NR(SCCP_IOCTCONFIG):
	case _IOC_NR(SCCP_IOCCCONFIG):
		err = sccp_size_conf(dp->b_rptr, &size);
		break;
	case _IOC_NR(SCCP_IOCLCONFIG):
		if (bp == NULL)
			goto enobufs;
		err = sccp_lst_conf(bp);
		break;
	case _IOC_NR(SCCP_IOCGSTATEM):
		if (bp == NULL)
			goto enobufs;
		err = sccp_get_statem(bp);
		break;
	case _IOC_NR(SCCP_IOCCMRESET):
		err = EOPNOTSUPP;	/* later */
		break;
	case _IOC_NR(SCCP_IOCGSTATSP):
		if (bp == NULL)
			goto enobufs;
		err = sccp_do_statsp(bp, SCCP_GET);
		break;
	case _IOC_NR(SCCP_IOCSSTATSP):
		err = sccp_size_stats(dp->b_rptr, &size);
		break;
	case _IOC_NR(SCCP_IOCGSTATS):
		if (bp == NULL)
			goto enobufs;
		err = sccp_get_stats(bp, false);
		break;
	case _IOC_NR(SCCP_IOCCSTATS):
		if (bp == NULL)
			goto enobufs;
		err = sccp_get_stats(bp, true);
		break;
	case _IOC_NR(SCCP_IOCGNOTIFY):
		if (bp == NULL)
			goto enobufs;
		err = sccp_do_notify(bp, 0, SCCP_GET);
		break;
	case _IOC_NR(SCCP_IOCSNOTIFY):
	case _IOC_NR(SCCP_IOCCNOTIFY):
		err = sccp_size_notify(dp->b_rptr, &size);
		break;
	case _IOC_NR(SCCP_IOCCMGMT):
		err = sccp_action(q, dp);
		break;
	case _IOC_NR(SCCP_IOCCPASS):
		break;
	default:
		err = EOPNOTSUPP;
		break;
	      enobufs:
		err = ENOBUFS;
		break;
	}
      done:
	if (err < 0)
		return (err);
	if (err > 0) {
		mc_copy_done(mc, q, mp, err);
		return (0);
	}
	if (size == -1)
		mi_copyout(mc, q, mp);
	else
		mi_copyin_n(mc, q, mp, 0, size);
	return (0);
}
static int
mc_sccp_copyin2(struct mc *mc, queue_t *q, mblk_t *mp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	int err = 0;
	mblk_t *bp;

	if (!(bp = mi_copyout_alloc(q, mp, NULL, dp->b_wptr - dp->b_rptr, false)))
		goto enobufs;
	bcopy(dp->b_rptr, bp->b_rptr, dp->b_wptr - dp->b_rptr);

	switch (_IOC_NR(cp->cp_cmd)) {
	case _IOC_NR(SCCP_IOCGOPTION):
		err = EPROTO;
		break;
	case _IOC_NR(SCCP_IOCSOPTION):
		err = sccp_set_options(bp);
		break;
	case _IOC_NR(SCCP_IOCGCONFIG):
		err = EPROTO;
		break;
	case _IOC_NR(SCCP_IOCSCONFIG):
		err = sccp_set_conf(bp);
		break;
	case _IOC_NR(SCCP_IOCTCONFIG):
		err = sccp_test_conf(bp);
		break;
	case _IOC_NR(SCCP_IOCCCONFIG):
		err = sccp_commit_conf(bp);
		break;
#ifdef SCCP_IOCLCONFIG
	case _IOC_NR(SCCP_IOCLCONFIG):
		err = EPROTO;
		break;
#endif				/* SCCP_IOCLCONFIG */
	case _IOC_NR(SCCP_IOCGSTATEM):
		err = EPROTO;
		break;
	case _IOC_NR(SCCP_IOCCMRESET):
		err = EPROTO;
		break;
	case _IOC_NR(SCCP_IOCGSTATSP):
		err = EPROTO;
		break;
	case _IOC_NR(SCCP_IOCSSTATSP):
		err = sccp_do_statsp(bp, SCCP_CHA);
		break;
	case _IOC_NR(SCCP_IOCGSTATS):
		err = EPROTO;
		break;
	case _IOC_NR(SCCP_IOCCSTATS):
		err = EPROTO;
		break;
	case _IOC_NR(SCCP_IOCGNOTIFY):
		err = EPROTO;
		break;
	case _IOC_NR(SCCP_IOCSNOTIFY):
		err = sccp_do_notify(bp, 0, SCCP_ADD);
		break;
	case _IOC_NR(SCCP_IOCCNOTIFY):
		err = sccp_do_notify(bp, 0, SCCP_DEL);
		break;
	case _IOC_NR(SCCP_IOCCMGMT):
		err = EPROTO;
		break;
	case _IOC_NR(SCCP_IOCCPASS):
	{
		struct sccp_pass *p = (typeof(p)) dp->b_rptr;

		break;
	}
	default:
		err = EOPNOTSUPP;
		break;
	}
      done:
	if (err < 0)
		return (err);
	if (err > 0)
		mc_copy_done(mc, q, mp, err);
	if (err == 0)
		mc_copyout(mc, q, mp);
	return (0);
      enobufs:
	err = ENOBUFS;
	goto done;
}
static int
mc_sccp_copyout(struct mc *mc, queue_t *q, mblk_t *mp)
{
	mc_copyout(mc, q, mp);
	return (0);
}

/**
 * mc_other_ioctl: - process other input-output controls
 * @mc: MAP private structure
 * @q: active queue (upper write queue)
 * @mp: the input-output control message
 *
 * This function simply passes input-output controls that are not for our level down to the lower
 * level drivers beneath the multiplexing driver.  The input-output control command and id are
 * stored against the upper private structure so that input-output control responses are passed to
 * the correct originating stream.
 */
static int
mc_other_ioctl(struct mc *mc, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct tc *tc = NULL;
	int err;

	if ((err = mc_get_lower(mc, &tc)) == 0) {
		mc->ioc_cmd = ioc->ioc_cmd;
		mc->ioc_id = ioc->ioc_id;
		LOGTX(tc, "%s: %s", __FUNCTION__, mc_iocname(ioc->ioc_cmd));
		putnext(tc->wq, mp);
		mc_put_lower(mc, tc);
	}
	return (err);
}

/**
 * tc_other_ioctl: = process other input-output controls
 * @tc: TC private structure
 * @q: active queue (lower read queue)
 * @mp: the input-output control message
 *
 * This function is the other half of mc_other_ioctl(): it passes input-output control responses
 * that are not for use to the upper level uers above the multiplexing driver.  The input-output
 * control command and id are compared against those stored in the upper private structure so that
 * input-output control responess are passed to the correct originating stream.
 */
static int
tc_other_ioctl(struct tc *tc, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct mc *mc = NULL;
	int err = EINVAL;

	read_lock(&mc_lock);
	for (mc = tc->mc_list; mc; mc = mc->tc_next) {
		if (mc->ioc_cmd == ioc->ioc_cmd && mc->ioc_id == ioc->ioc_id) {
			putnext(mc->rq, mp);
			err = 0;
			break;
		}
	}
	read_unlock(&mc_lock);
	return (err);
}

/*
 * M_IOCTL HANDLING
 * --------------------------------------------------------------------------
 */
noinline fastcall __unlikely int
__mc_w_ioctl(struct mc *mc, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct mc *mc;
	int err;

	LOGIO(mc, "-> M_IOCTL(%s)", mc_iocname(ioc->ioc_cmd));

	if (likely(!!mp->b_cont)) {
		switch (_IOC_TYPE(ioc->ioc_cmd)) {
		case __SID:
			err = mc_i_ioctl(mc, q, mp);
			break;
		case MAP_IOC_MAGIC:
			err = mc_map_ioctl(mc, q, mp);
			break;
		case TCAP_IOC_MAGIC:
			err = mc_tcap_ioctl(mc, q, mp);
			break;
		case SCCP_IOC_MAGIC:
			err = mc_sccp_ioctl(mc, q, mp);
			break;
		default:
			/* pass all others down */
			err = mc_other_ioctl(mc, q, mp);
			break;
		}
	} else
		err = -EFAULT;
	if (err < 0) {
		switch (-err) {
		case -EAGAIN:
		case -ENOMEM:
		case -ENOBUFS:
		case -EBUSY:
		case -EDEADLK:
			break;
		default:
			mc_copy_done(mc, q, mp, -err);
			return (0);
		}
	}
	return (err);
}
noinline fastcall __unlikely int
__tc_r_ioctl(struct tc *tc, queue_t *q, mblk_t *mp)
{
}
noinline fastcall __unlikely int
mc_w_ioctl(queue_t *q, mblk_t *mp)
{
	struct mc *mc;
	int err;

	if (likely(!!(mc = mc_trylock(q)))) {
		err = __mc_w_ioctl(mc, q, mp);
		mc_unlock(mc);
	} else
		err = -EDEADLK;
	return (err);
}
noinline fastcall __unlikely int
tc_r_ioctl(queue_t *q, mblk_t *mp)
{
	struct tc *tc;
	int err;

	if (likely(!!(tc = tc_trylock(q)))) {
		err = __tc_r_ioctl(tc, q, mp);
		tc_unlock(tc);
	} else
		err = -EDEADLK;
	return (err);
}

/*
 * M_IOCDATA HANDLING
 * --------------------------------------------------------------------------
 */
noinline fastcall __unlikely int
__mc_w_iocdata(struct mc *mc, queue_t *q, mblk_t *mp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	mblk_t *dp;
	int err;

	LOGIO(mc, "-> M_IOCDATA(%s)", mc_iocname(cp->cp_cmd));

	switch (_IOC_TYPE(cp->cp_cmd)) {
	case MAP_IOC_MAGIC:
		switch (mi_copy_state(q, mp, &dp)) {
		case MI_COPY_CASE(MI_COPY_IN, 1):
			err = mc_map_copyin(mc, q, mp, dp);
			break;
		case MI_COPY_CASE(MI_COPY_IN, 2):
			err = mc_map_copyin2(mc, q, mp, dp);
			break;
		case MI_COPY_CASE(MI_COPY_OUT, 1):
			err = mc_map_copyout(mc, q, mp, dp);
			break;
		case -1:
			return (0);
		default:
			err = -EPROTO;
			break;
		}
		break;
	case TCAP_IOC_MAGIC:
		switch (mi_copy_state(q, mp, &dp)) {
		case MI_COPY_CASE(MI_COPY_IN, 1):
			err = mc_tcap_copyin(mc, q, mp, dp);
			break;
		case MI_COPY_CASE(MI_COPY_IN, 2):
			err = mc_tcap_copyin2(mc, q, mp, dp);
			break;
		case MI_COPY_CASE(MI_COPY_OUT, 1):
			err = mc_tcap_copyout(mc, q, mp, dp);
			break;
		case -1:
			return (0);
		default:
			err = -EPROTO;
			break;
		}
		break;
	case SCCP_IOC_MAGIC:
		switch (mi_copy_state(q, mp, &dp)) {
		case MI_COPY_CASE(MI_COPY_IN, 1):
			err = mc_sccp_copyin(mc, q, mp, dp);
			break;
		case MI_COPY_CASE(MI_COPY_IN, 2):
			err = mc_sccp_copyin2(mc, q, mp, dp);
			break;
		case MI_COPY_CASE(MI_COPY_OUT, 1):
			err = mc_sccp_copyout(mc, q, mp, dp);
			break;
		case -1:
			return (0);
		default:
			err = -EPROTO;
			break;
		}
		break;
	default:
		/* pass all others down */
		err = mc_other_ioctl(mc, q, mp);
		break;
	}
	if (err < 0) {
		switch (-err) {
		case -EAGAIN:
		case -ENOMEM:
		case -ENOBUFS:
		case -EBUSY:
		case -EDEADLK:
			break;
		default:
			mc_copy_done(mc, q, mp, -err);
			return (0);
		}
	}
	return (err);
}
noinline fastcall __unlikely int
__tc_r_iocdata(struct tc *tc, queue_t *q, mblk_t *mp)
{
}
noinline fastcall __unlikely int
mc_w_iocdata(queue_t *q, mblk_t *mp)
{
	struct mc *mc;
	int err;

	if (likely(!!(mc = mc_trylock(q)))) {
		err = __mc_w_iocdata(mc, q, mp);
		mc_unlock(mc);
	} else
		err = -EDEADLK;
	return (err);
}
noinline fastcall __unlikely int
tc_r_iocdata(queue_t *q, mblk_t *mp)
{
	struct tc *tc;
	int err;

	if (likely(!!(tc = tc_trylock(q)))) {
		err = __tc_r_iocdata(tc, q, mp);
		tc_unlock(tc);
	} else
		err = -EDEADLK;
	return (err);
}

/*
 * M_CTL AND M_PCCTL HANDLING
 * --------------------------------------------------------------------------
 */
static inline fastcall __hot_out int
__mc_w_ctl(struct mc *mc, queue_t *q, mblk_t *mp)
{
}
static inline fastcall __hot_get int
__tc_r_ctl(struct tc *tc, queue_t *q, mblk_t *mp)
{
}
static inline fastcall __hot_put int
mc_w_ctl(queue_t *q, mblk_t *mp)
{
	struct mc *mc;
	int err;

	if (likely(!!(mc = mc_trylock(q)))) {
		err = __mc_w_ctl(mc, q, mp);
		mc_unlock(mc);
	} else
		err = -EDEADLK;
	return (err);
}
static inline fastcall __hot_in int
tc_r_ctl(queue_t *q, mblk_t *mp)
{
	struct tc *tc;
	int err;

	if (likely(!!(tc = tc_trylock(q)))) {
		err = __tc_r_ctl(tc, q, mp);
		tc_unlock(tc);
	} else
		err = -EDEADLK;
	return (err);
}

/*
 * M_PROTO AND M_PCPROTO HANDLING
 * --------------------------------------------------------------------------
 */
static noinline __unlikely int
mc_w_proto_return(struct mc *mc, queue_t *q, mblk_t *mp, int err)
{
	t_scalar_t type = *(t_scalar_t *) mp->b_rptr;

	if (err < 0)
		mc_restore_state(mc);
	switch (__builtin_expect(-err, EDEADLK)) {
	case EBUSY:
	case EAGAIN:
	case ENOMEM:
	case ENOBUFS:
	case EDEADLK:
		return (err);
	default:
		freemsg(mp);
		/* fall through */
	case 0:
		LOGNO(mc, "Primitive <%s> on upper write queue generated error [%s]",
		      mc_primname(type), mc_errname(err));
		return (0);
	case EPROTO:
	case EFAULT:
		return m_error(mc, q, mp, -err);
	}
}
static noinline __unlikely int
tc_r_proto_return(struct tc *tc, queue_t *q, mblk_t *mp, int err)
{
	t_scalar_t type = *(t_scalar_t *) mp->b_rptr;

	if (err < 0)
		tc_restore_state(tc);
	switch (__builtin_expect(-err, EDEADLK)) {
	case EBUSY:
	case EAGAIN:
	case ENOMEM:
	case ENOBUFS:
	case EDEADLK:
		return (err);
	default:
		freemsg(mp);
		/* fall through */
	case 0:
		LOGERR(tc, "Primitive <%s> on lower read queue generated error [%s]",
		       tc_primname(type), tc_errname(err));
		return (0);
	case EPROTO:
	case EFAULT:
		return m_error_ind(tc, q, mp, -err);
	}

}
static inline fastcall __hot_out int
__mc_w_proto_slow(struct mc *mc, queue_t *q, mblk_t *mp)
{
	union N_primitives *p = (typeof(p)) mp->b_rptr;
	register t_scalar_t type = p->type;
	int err;

	mc_save_state(mc);
	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto emsgsize;
	LOGRX(mc, "-> %s", mc_primname(type));
	switch (type) {
	case MAP_INFO_REQ:
		err = mc_info_req(mc, q, mp, &p->info_req);
		break;
	case MAP_BIND_REQ:
		err = mc_bind_req(mc, q, mp, &p->bind_req);
		break;
	case MAP_UNBIND_REQ:
		err = mc_unbind_req(mc, q, mp, &p->unbind_req);
		break;
	case MAP_OPTMGMT_REQ:
		err = mc_optmgmt_req(mc, q, mp, &p->optmgmt_req);
		break;
	case MAP_OPEN_REQ:
		err = mc_open_req(mc, q, mp, &p->open_req);
		break;
	case MAP_OPEN_RES:
		err = mc_open_res(mc, q, mp, &p->open_res);
		break;
	case MAP_REFUSE_RES:
		err = mc_refuse_res(mc, q, mp, &p->refuse_res);
		break;
	case MAP_SERV_REQ:
		err = mc_serv_req(mc, q, mp, &p->serv_req);
		break;
	case MAP_SERV_RES:
		err = mc_serv_res(mc, q, mp, &p->serv_res);
		break;
	case MAP_DELIM_REQ:
		err = mc_delim_req(mc, q, mp, &p->delim_req);
		break;
	case MAP_CLOSE_REQ:
		err = mc_close_req(mc, q, mp, &p->close_req);
		break;
	case MAP_ABORT_REQ:
		err = mc_abort_req(mc, q, mp, &p->abort_req);
		break;
	case MAP_STC1_REQ:
		err = mc_stc1_req(mc, q, mp, &p->stc1_req);
		break;
	case MAP_STC2_REQ:
		err = mc_stc2_req(mc, q, mp, &p->stc2_req);
		break;
	case MAP_STC3_REQ:
		err = mc_stc3_req(mc, q, mp, &p->stc3_req);
		break;
	case MAP_STC4_REQ:
		err = mc_stc4_req(mc, q, mp, &p->stc4_req);
		break;
	default:
		err = mc_other_req(mc, q, mp, &p->type);
		break;
	}
	if (err)
		goto error;
	return (0);
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	if (err < 0)
		mc_restore_state(mc);
	return (err);
}
static fastcall __hot_get int
__tc_r_proto_slow(struct tc *tc, queue_t *q, mblk_t *mp)
{
	union TR_primitives *p = (typeof(p)) mp->b_rptr;
	register t_scalar_t type = p->type;
	int err;

	tc_save_state(tc);
	if (unlikely(!MBLKIN(mp, 0, sizeof(type))))
		goto emsgsize;
	LOGRX(tc, "%s <-", tc_primname(type));
	switch (type) {
	case TC_INFO_ACK:
		err = tc_info_ack(tc, q, mp, &p->info_ack);
		break;
	case TC_BIND_ACK:
		err = tc_bind_ack(tc, q, mp, &p->bind_ack);
		break;
	case TC_SUBS_BIND_ACK:
		err = tc_subs_bind_ack(tc, q, mp, &p->subs_bind_ack);
		break;
	case TC_OK_ACK:
		err = tc_ok_ack(tc, q, mp, &p->ok_ack);
		break;
	case TC_ERROR_ACK:
		err = tc_error_ack(tc, q, mp, &p->error_ack);
		break;
	case TC_OPTMGMT_ACK:
		err = tc_optmgmt_ack(tc, q, mp, &p->optmgmt_ack);
		break;
	case TC_UNI_IND:
		err = tc_uni_ind(tc, q, mp, &p->uni_ind);
		break;
	case TC_BEGIN_IND:
		err = tc_begin_ind(tc, q, mp, &p->begin_ind);
		break;
	case TC_BEGIN_CON:
		err = tc_begin_con(tc, q, mp, &p->begin_con);
		break;
	case TC_CONT_IND:
		err = tc_cont_ind(tc, q, mp, &p->cont_ind);
		break;
	case TC_END_IND:
		err = tc_end_ind(tc, q, mp, &p->end_ind);
		break;
	case TC_ABORT_IND:
		err = tc_abort_ind(tc, q, mp, &p->abort_ind);
		break;
	case TC_NOTICE_IND:
		err = tc_notice_ind(tc, q, mp, &p->notice_ind);
		break;
	case TC_INVOKE_IND:
		err = tc_invoke_ind(tc, q, mp, &p->invoke_ind);
		break;
	case TC_RESULT_IND:
		err = tc_result_ind(tc, q, mp, &p->result_ind);
		break;
	case TC_ERROR_IND:
		err = tc_error_ind(tc, q, mp, &p->error_ind);
		break;
	case TC_CANCEL_IND:
		err = tc_cancel_ind(tc, q, mp, &p->cancel_ind);
		break;
	case TC_REJECT_IND:
		err = tc_reject_ind(tc, q, mp, &p->reject_ind);
		break;
	default:
		err = tc_other_ind(tc, q, mp, &p->type);
		break;
	}
	if (err)
		goto error;
	return (0);
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	if (err < 0)
		tc_restore_state(tc);
	return (err);
}
static inline fastcall __hot_out int
__mc_w_proto(struct mc *mc, queue_t *q, mblk_t *mp)
{
	union MAP_primitives *p = (typeof(p)) mp->b_rptr;
	register t_scalar_t type = p->type;
	register int err;

	if ((1 << type) & ((1 << MAP_OPEN_REQ) | (1 << MAP_OPEN_RES) | (1 << MAP_SERV_REQ) |
			   (1 << MAP_SERV_RES) | (1 << MAP_DELIM_REQ) | (1 << MAP_CLOSE_REQ) |
			   (1 << MAP_OPEN_IND) | (1 << MAP_OPEN_CON) | (1 << MAP_REFUSE_IND) |
			   (1 << MAP_SERV_IND) | (1 << MAP_SERV_CON) | (1 << MAP_DELIM_IND) |
			   (1 << MAP_CLOSE_IND))) {
		/* fast paths for data */
		switch (type) {
		case MAP_OPEN_REQ:
			if (unlikely((err = mc_open_req(mc, q, mp, &p->open_req))))
				goto error;
			break;
		case MAP_OPEN_RES:
			if (unlikely((err = mc_open_res(mc, q, mp, &p->open_res))))
				goto error;
			break;
		case MAP_SERV_REQ:
			if (unlikely((err = mc_serv_req(mc, q, mp, &p->serv_req))))
				goto error;
			break;
		case MAP_SERV_RES:
			if (unlikely((err = mc_serv_res(mc, q, mp, &p->serv_res))))
				goto error;
			break;
		case MAP_DELIM_REQ:
			if (unlikely((err = mc_delim_req(mc, q, mp, &p->delim_req))))
				goto error;
			break;
		case MAP_CLOSE_REQ:
			if (unlikely((err = mc_close_req(mc, q, mp, &p->close_req))))
				goto error;
			break;
		case MAP_OPEN_IND:
			if (unlikely((err = mc_open_ind(mc, q, mp, &p->open_ind))))
				goto error;
			break;
		case MAP_OPEN_CON:
			if (unlikely((err = mc_open_con(mc, q, mp, &p->open_con))))
				goto error;
			break;
		case MAP_REFUSE_IND:
			if (unlikely((err = mc_refuse_ind(mc, q, mp, &p->refuse_ind))))
				goto error;
			break;
		case MAP_SERV_IND:
			if (unlikely((err = mc_serv_ind(mc, q, mp, &p->serv_ind))))
				goto error;
			break;
		case MAP_SERV_CON:
			if (unlikely((err = mc_serv_con(mc, q, mp, &p->serv_con))))
				goto error;
			break;
		case MAP_DELIM_IND:
			if (unlikely((err = mc_delim_ind(mc, q, mp, &p->delim_ind))))
				goto error;
			break;
		case MAP_CLOSE_IND:
			if (unlikely((err = mc_close_ind(mc, q, mp, &p->close_ind))))
				goto error;
			break;
		}
	} else if (unlikely((err = __mc_w_proto_slow(mc, q, mp))))
		goto error;
	return (0);
      error:
	return mc_w_proto_return(mc, q, mp, err);
}
static inline fastcall __hot_get int
__tc_r_proto(struct tc *tc, queue_t *q, mblk_t *mp)
{
	union TR_primitives *p = (typeof(p)) mp->b_rptr;
	register t_scalar_t type = p->type;
	register int err;

	if (type < 32) {
		if ((1 << type) & ((1 << TC_UNI_REQ) | (1 << TC_BEGIN_REQ) | (1 << TC_BEGIN_RES) |
				   (1 << TC_CONT_REQ) | (1 << TC_END_REQ) | (1 << TC_UNI_IND) |
				   (1 << TC_BEGIN_IND) | (1 << TC_BEGIN_CON) | (1 << TC_CONT_IND) |
				   (1 << TC_END_IND)))
			switch (type) {
			case TC_UNI_REQ:
				if (unlikely((err = tc_uni_req(tc, q, mp, &p->uni_req))))
					goto error;
				break;
			case TC_BEGIN_REQ:
				if (unlikely((err = tc_begin_req(tc, q, mp, &p->begin_req))))
					goto error;
				break;
			case TC_BEGIN_RES:
				if (unlikely((err = tc_begin_res(tc, q, mp, &p->begin_res))))
					goto error;
				break;
			case TC_CONT_REQ:
				if (unlikely((err = tc_cont_req(tc, q, mp, &p->cont_req))))
					goto error;
				break;
			case TC_END_REQ:
				if (unlikely((err = tc_end_req(tc, q, mp, &p->end_req))))
					goto error;
				break;
			case TC_UNI_IND:
				if (unlikely((err = tc_uni_ind(tc, q, mp, &p->uni_ind))))
					goto error;
				break;
			case TC_BEGIN_IND:
				if (unlikely((err = tc_begin_ind(tc, q, mp, &p->begin_ind))))
					goto error;
				break;
			case TC_BEGIN_CON:
				if (unlikely((err = tc_begin_con(tc, q, mp, &p->begin_con))))
					goto error;
				break;
			case TC_CONT_IND:
				if (unlikely((err = tc_cont_ind(tc, q, mp, &p->cont_ind))))
					goto error;
				break;
			case TC_END_IND:
				if (unlikely((err = tc_end_ind(tc, q, mp, &p->end_ind))))
					goto error;
				break;
			}
	} else if (type >= 48 && type < 80) {
		if ((1 << type) & ((1 << TC_INVOKE_REQ) | (1 << TC_RESULT_REQ) |
				   (1 << TC_INVOKE_IND) | (1 << TC_RESULT_IND)))
			switch (type) {
			case TC_INVOKE_REQ:
				if (unlikely((err = tc_invoke_req(tc, q, mp, &p->invoke_req))))
					goto error;
				break;
			case TC_RESULT_REQ:
				if (unlikely((err = tc_result_req(tc, q, mp, &p->result_req))))
					goto error;
				break;
			case TC_INVOKE_IND:
				if (unlikely((err = tc_invoke_id(tc, q, mp, &p->invoke_id))))
					goto error;
				break;
			case TC_RESULT_IND:
				if (unlikely((err = tc_result_ind(tc, q, mp, &p->result_ind))))
					goto error;
				break;
			}
	} else if (unlikely((err = __tc_r_proto_slow(tc, q, mp))))
		goto error;
	return (0);
      error:
	return tr_r_proto_return(tc, q, mp, err);
}
noinline fastcall int
mc_w_proto_slow(queue_t *q, mblk_t *mp)
{
	struct mc *mc;
	int err;

	if (likely(!!(mc = mc_trylock(q)))) {
		err = __mc_w_proto_slow(mc, q, mp);
		mc_unlock(mc);
	} else
		err = -EDEADLK;
	return (err);
}
noinline fastcall int
tc_r_proto_slow(queue_t *q, mblk_t *mp)
{
	struct tc *tc;
	int err;

	if (likely(!!(tc = tc_trylock(q)))) {
		err = __tc_r_proto_slow(tc, q, mp);
		tc_unlock(tc);
	} else
		err = -EDEADLK;
	return (err);
}
static inline fastcall __hot_put int
mc_w_proto(queue_t *q, mblk_t *mp)
{
	union MAP_primitives *p = (typeof(p)) mp->b_rptr;

	if (likely(MBLKIN(mp, 0, sizeof(p->type)))) {
		register t_scalar_t type = p->type;

		if ((1 << type) & ((1 << MAP_OPEN_REQ) | (1 << MAP_OPEN_RES) |
				   (1 << MAP_SERV_REQ) | (1 << MAP_SERV_RES) |
				   (1 << MAP_DELIM_REQ) | (1 << MAP_CLOSE_REQ) |
				   (1 << MAP_OPEN_IND) | (1 << MAP_OPEN_CON) |
				   (1 << MAP_REFUSE_IND) | (1 << MAP_SERV_IND) |
				   (1 << MAP_SERV_CON) | (1 << MAP_DELIM_IND) |
				   (1 << MAP_CLOSE_IND)))
			return (-EAGAIN);
	}
	return mc_w_proto_slow(q, mp);
}
static inline fastcall __hot_in int
tc_r_proto(queue_t *q, mblk_t *mp)
{
	union TR_primitives *p = (typeof(p)) mp->b_rptr;

	if (likely(MBLKIN(mp, 0, sizeof(p->type)))) {
		register t_scalar_t type = p->type;

		if (type < 32) {
			if ((1 << type) & ((1 << TC_UNI_REQ) | (1 << TC_BEGIN_REQ) |
					   (1 << TC_BEGIN_RES) | (1 << TC_CONT_REQ) |
					   (1 << TC_END_REQ) | (1 << TC_UNI_IND) |
					   (1 << TC_BEGIN_IND) | (1 << TC_BEGIN_CON) |
					   (1 << TC_CONT_IND) | (1 << TC_END_IND)))
				return (-EAGAIN);
		} else if (type >= 48 && type < 80) {
			if ((1 << type) & ((1 << TC_INVOKE_REQ) | (1 << TC_RESULT_REQ) |
					   (1 << TC_INVOKE_IND) | (1 << TC_RESULT_IND)))
				return (-EAGAIN);
		}
	}
	return tc_r_proto_slow(q, mp);
}

/*
 * M_DATA AND M_HPDATA HANDLING
 * --------------------------------------------------------------------------
 */
static inline fastcall __hot_out int
__mc_w_data(struct mc *mc, queue_t *q, mblk_t *mp)
{
}
static inline fastcall __hot_read int
__tc_r_data(struct tc *tc, queue_t *q, mblk_t *mp)
{
}
static inline fastcall __hot_write int
mc_w_data(queue_t *q, mblk_t *mp)
{
	if (unlikely(pcmsg(DB_TYPE(mp)))) {
		struct mc *mc;
		int err = -EDEADLK;

		if (!!(mc = mc_trylock(q))) {
			err = __mc_w_data(mc, q, mp);
			mc_unlock(mc);
		}
		return (err);
	}
	return (-EAGAIN);	/* always queue normal data for speed */
}
static inline fastcall __hot_in int
tc_r_data(queue_t *q, mblk_t *mp)
{
	if (unlikely(pcmsg(DB_TYPE(mp)))) {
		struct tc *tc;
		int err = -EDEADLK;

		if (!!(tc = tc_trylock(q))) {
			err = __mc_r_data(tc, q, mp);
			tc_unlock(tc);
		}
		return (err);
	}
	return (-EAGAIN);	/* always queue normal data for speed */
}

/*
 * M_HANGUP HANDLING
 * --------------------------------------------------------------------------
 */
noinline fastcall __unlikely int
tc_r_hangup(queue_t *q, mblk_t *mp)
{
	return (-EOPNOTSUPP);	/* FIXME */
}

/*
 * M_ERROR HANDLING
 * --------------------------------------------------------------------------
 */
noinline fastcall __unlikely int
tc_r_error(queue_t *q, mblk_t *mp)
{
	return (-EOPNOTSUPP);	/* FIXME */
}

/*
 * M_FLUSH HANDLING
 * --------------------------------------------------------------------------
 */
noinline fastcall __unlikely int
mc_w_flush(queue_t *q, mblk_t *mp)
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
	} else
		freemsg(mp);
	return (0);
}
noinline fastcall __unlikely int
tc_r_flush(queue_t *q, mblk_t *mp)
{
	if (mp->b_rptr[0] & FLUSHR) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
		mp->b_rptr[0] &= ~FLUSHR;
	}
	if (mp->b_rptr[0] & FLUSHW) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(WR(q), mp->b_rptr[1], FLUSHDATA);
		else
			flushq(WR(q), FLUSHDATA);
		if (!(mp->b_flag & MSGNOLOOP)) {
			mp->b_flag |= MSGNOLOOP;
			qreply(q, mp);
		} else
			freemsg(mp);
	} else
		freemsg(mp);
	return (0);
}

/*
 * OTHER STREAMS MESSAGE HANDLING
 * --------------------------------------------------------------------------
 */
noinline fastcall __unlikely int
mc_w_other(queue_t *q, mblk_t *mp)
{
	if (likely(pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band))) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}
noinline fastcall __unlikely int
tc_r_other(queue_t *q, mblk_t *mp)
{
	if (likely(pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band))) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

/*
 * STREAMS MESSAGE DISCRIMINATION
 * --------------------------------------------------------------------------
 */
static fastcall int
mc_wput_msg(queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(DB_TYPE(mp), M_PROTO)) {
	case M_PROTO:
	case M_PCPROTO:
		return mc_w_proto(q, mp);
	case M_DATA:
		return mc_w_data(q, mp);
	case M_FLUSH:
		return mc_w_flush(q, mp);
	default:
		return mc_w_other(q, mp);
	}
}
static fastcall int
mc_wsrv_msg(struct mc *mc, queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(DB_TYPE(mp), M_PROTO)) {
	case M_PROTO:
	case M_PCPROTO:
		return __mc_w_proto(mc, q, mp);
	case M_DATA:
		return __mc_w_data(mc, q, mp);
	default:
		return mc_w_other(q, mp);
	}
}
static fastcall int
tc_rsrv_msg(struct tc *tc, queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(DB_TYPE(mp), M_PROTO)) {
	case M_PROTO:
	case M_PCPROTO:
		return __tc_r_proto(tc, q, mp);
	case M_DATA:
		return __tc_r_data(tc, q, mp);
	default:
		return tc_r_other(q, mp);
	}
}
static fastcall int
tc_rput_msg(queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(DB_TYPE(mp), M_PROTO)) {
	case M_PROTO:
	case M_PCPROTO:
		return tc_r_proto(q, mp);
	case M_DATA:
		return tc_r_data(q, mp);
	case M_FLUSH:
		return tc_r_flush(q, mp);
	case M_ERROR:
		return tc_r_error(q, mp);
	case M_HANGUP:
		return tc_r_hangup(q, mp);
	default:
		return tc_r_other(q, mp);
	}
}

/*
 * QUEUE PUT AND SERVICE PROCEDURES
 * --------------------------------------------------------------------------
 */
static streamscall int
mc_wput(queue_t *q, mblk_t *mp)
{
	if (unlikely(!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY)))
	    || unlikely(mc_wput_msg(q, mp))) {
		mc_mstat.ms_acnt++;
		if (unlikely(!putq(q, mp))) {
			mp->b_band = 0;
			putq(q, mp);	/* cannot fail */
		}
	}
	return (0);
}
static streamscall int
mc_wsrv(queue_t *q)
{
	struct mc *mc;
	mblk_t *mp;

	if (likely(!!(mc = mc_trylock(q)))) {
		while (likely(!!(mp = getq(q)))) {
			if (unlikely(mc_wsrv_msg(mc, q, mp))) {
				if (unlikely(!putbq(q, mp))) {
					mp->b_band = 0;	/* must succeed */
					putbq(q, mp);
				}
				LOGTX(mc, "write queue stalled");
				break;
			}
		}
		mc_unlock(mc);
	}
	return (0);
}
static streamscall int
mc_rsrv(queue_t *q)
{
	struct mc *mc;

	if (likely(!!(mc = mc_trylock(q)))) {
		struct tc *tc = NULL;

		if (mc_get_lower(mc, &tc) == 0) {
			/* back-enable across multiplexer */
			qenable(tc->rq);
			mc_put_lower(mc, tc);
		}
		mc_unlock(mc);
	}
	return (0);
}
static streamscall int
tc_wsrv(queue_t *q)
{
	struct tc *tc;

	if (likely(!!(tc = tc_trylock(q)))) {
		struct mc *mc = NULL;

		if (tc_get_upper(tc, &mc) == 0) {
			/* back-enable across multiplexer */
			qenable(mc->wq);
			tc_put_upper(tc, mc);
		}
		tc_unlock(tc);
	}
	return (0);
}
static streamscall int
tc_rsrv(queue_t *q)
{
	struct tc *tc;
	mblk_t *mp;

	if (likely(!!(tc = tc_trylock(q)))) {
		while (likely(!!(mp = getq(q)))) {
			if (unlikely(tc_rsrv_msg(tc, q, mp))) {
				if (unlikely(!putbq(q, mp))) {
					mp->b_band = 0;	/* must succeed */
					putbq(q, mp);
				}
				LOGTX(tc, "read queue stalled");
				break;
			}
		}
		tc_unlock(tc);
	}
	return (0);
}
static streamscall int
tc_rput(queue_t *q, mblk_t *mp)
{
	if (unlikely(!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY)))
	    || unlikely(tc_rput_msg(q, mp))) {
		tc_mstat.ms_acnt++;
		if (unlikely(!putq(q, mp))) {
			mp->b_band = 0;
			putq(q, mp);	/* cannot fail */
		}
	}
	return (0);
}

/*
 * QUEUE OPEN and CLOSE routines
 * --------------------------------------------------------------------------
 */
static streamscall int
mc_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	unsigned long flags;
	struct mc *mc;
	caddr_t ptr;
	int err;
	minor_t cminor = getminor(*devp);

	if (q->q_ptr)
		return (0);	/* already open */
	if (sflag == MODOPEN || WR(q)->q_next != NULL)
		goto enxio;	/* can't be pushed as module */
	if (cminor < FIRST_CMINOR || cminor > LAST_CMINOR)
		goto enxio;
	if (!mi_set_sth_lowat(q, 0))
		goto enobufs;
	if (!mi_set_sth_hiwat(q, SHEADHIWAT >> 1))
		goto enobufs;
	if (!(mc = (struct mc *) (ptr = mi_open_alloc_cache(priv_cachep, GFP_KERNEL))))
		goto enomem;
	bzero(mc, sizeof(*mc));
	mc->rq = RD(q);
	mc->wq = WR(q);
	mc->cred = *crp;
	mc->dev = *devp;
	sflag = CLONEOPEN;
	cminor = FREE_CMINOR;
	*devp = makedevice(getmajor(*devp), cminor);
	write_lock_irqsave(&mc_lock, flags);
	if (mi_acquire_sleep(ptr, &ptr, &mc_lock, &flags) == NULL) {
		err = EINTR;
		goto unlock_free;
	}
	if ((err = mi_open_link(&mc_opens, ptr, devp, oflags, sflag, crp))) {
		mi_release(ptr);
		goto unlock_free;
	}
	mi_attach(q, ptr);
	mi_release(ptr);
	write_unlock_irqrestore(&mc_lock, flags);
	mc_init(mc);
	qprocson(q);
	return (0);
      enxio:
	err = ENXIO;
	goto error;
      enobufs:
	err = ENOBUFS;
	goto error;
      enomem:
	err = ENOMEM;
	goto error;
      unlock_free:
	mi_close_free_cache(priv_cachep, ptr);
	write_unlock_irqrestore(&mc_lock, flags);
	goto error;
      error:
	freemsg(mp);
	return (err);
}
static streamscall int
mc_qclose(queue_t *q, int oflags, cred_t *crp)
{
	unsigned long flags;
	struct mc *mc = PRIV(q);
	caddr_t ptr = (caddr_t) mc;
	int err;

	write_lock_irqsave(&mc_lock, flags);
	mi_acquire_sleep_nosignal(ptr, &ptr, &mc_lock, &flags);
	qprocsoff(q);
	mc_term(mc);
	mc->rq = NULL;
	mc->wq = NULL;
	err = mi_close_comm(&mc_opens, q);
	write_unlock_irqrestore(&mc_lock, flags);
	if (err)
		cmn_err(CE_WARN, "%s: error closing stream, err = %d", __FUNCTION__, err);
	return (err);
}

/*
 * STREAMS DEFINITIONS
 * --------------------------------------------------------------------------
 */

static struct qinit mc_rinit = {
	/* never put to upper read queue */
	.qi_srvp = mc_rsrv,	/* for back-enable */
	.qi_qopen = mc_qopen,
	.qi_qclose = mc_qclose,
	.qi_minfo = &mc_minfo,
	.qi_mstat = &mc_mstat,
};
static struct qinit mc_winit = {
	.qi_putp = mc_wput,
	.qi_srvp = mc_wsrv,
	.qi_minfo = &mc_minfo,
	.qi_mstat = &mc_mstat,
};
static struct qinit tc_rinit = {
	.qi_putp = tc_rput,
	.qi_srvp = tc_rsrv,
	.qi_qopen = tc_link,
	.qi_qclose = tc_unlink,
	.qi_minfo = &tc_minfo,
	.qi_mstat = &tc_mstat,
};
static struct qinit tc_winit = {
	/* never put to lower write queue */
	.qi_srvp = tc_wsrv,	/* for back-enable */
	.qi_minfo = &tc_minfo,
	.qi_mstat = &tc_mstat,
};

MODULE_STATIC struct streamtab mcmuxinfo = {
	.st_rdinit = &mc_rinit,
	.st_wrinit = &mc_winit,
	.st_muxrinit = &tc_rinit,
	.st_muxwinit = &tc_winit,
};

STATIC struct cdevsw mc_cdev = {
	.d_name = DRV_NAME,
	.d_str = &mcmuxinfo,
	.d_flag = D_MP | D_CLONE,
	.d_fop = NULL,
	.d_mode = S_IFCHR | S_IRUGO | S_IWUGO,
	.d_kmod = THIS_MODULE,
};

/* mgr minor for management access */
static struct devnode mc_node_mgr = {
	.n_name = "mgr",
	.n_flag = D_CLONE,
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

/* mc minor for basic MAPI access */
static struct devnode mc_node_mc = {
	.n_name = "mc",
	.n_flag = D_CLONE,
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

/* tp minor for basic TPI access */
static struct devnode mc_node_tp = {
	.n_name = "tp",
	.n_flag = D_CLONE,
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};
static struct strapush mc_push_tp = {
	.ap_cmd = SAD_ONE,
	.sap_major = major,
	.sap_minor = TC_CMINOR,
	.sap_lastminor = TC_CMINOR,
	.sap_npush = 1,
	.sap_list = {
		     [0] = "tptr",
		     };
};

modID_t modid = DRV_ID;
major_t major = CMAJOR_0;

/*
 * Kernel module initialization
 * --------------------------------------------------------------------------
 */

MODULE_STATIC int __init
mcmuxinit(void)
{
	int err;

	cmn_err(CE_NOTE, DRV_BANNER);
	if ((err = mc_init_caches()) < 0) {
		cmn_err(CE_WARN, "%s: could not init caches, err = %d", __FUNCTION__, err);
		return (err);
	}
	if ((err = register_strdev(&mc_cdev, major)) < 0) {
		cmn_err(CE_WARN, "%s: could not register driver, err = %d", __FUNCTION__, err);
		goto no_major;
	}
	if ((err = register_strnod(&mc_cdev, &mc_node_mgr, MGR_CMINOR)) < 0) {
		cmn_err(CE_WARN, "%s: could not register MGR_CMINOR, err = %d", __FUNCTION__, err);
		goto no_mgr_cminor;
	}
	if ((err = register_strnod(&mc_cdev, &mc_node_mc, MC_CMINOR)) < 0) {
		cmn_err(CE_WARN, "%s: could not register MC_CMINOR, err = %d", __FUNCTION__, err);
		goto no_mc_cminor;
	}
	if ((err = register_strnod(&mc_cdev, &mc_node_tp, TP_CMINOR)) < 0) {
		cmn_err(CE_WARN, "%s: could not register TP_CMINOR, err = %d", __FUNCTION__, err);
		goto no_tp_cminor;
	}
	if ((err = autopush_add(&mc_push_tp)) < 0) {
		cmn_err(CE_WARN, "%s: could not add autopush for TP_CMINOR, err = %d", __FUNCTION__,
			err);
		goto no_apush_tp;
	}
	if (major == 0)
		major = err;
	if (major == 0)
		goto lost_major;
	return (0);
	/* error paths */
      lost_major:
	autopush_del(&mc_push_map);
      no_apush_map:
	autopush_del(&mc_push_tc);
      no_apush_tc:
	autopush_del(&mc_push_tp);
      no_apush_tp:
	unregister_strnod(&mc_cdev, TP_CMINOR);
      no_tp_cminor:
	unregister_strnod(&mc_cdev, MC_CMINOR);
      no_mc_cminor:
	unregister_strnod(&mc_cdev, MGR_CMINOR);
      no_mgr_cminor:
	unregister_strdev(&mc_cdev, major);
      no_major:
	mc_term_caches();
	return (err);
}

MODULE_STATIC void __exit
mcmuxexit(void)
{
	int err;

	if ((err = autopush_del(&mc_push_tp)) < 0)
		cmn_err(CE_WARN, "%s: could not delete autopush for TP_CMINOR, err = %d", __FUNCTION__, err);
	if ((err = unregister_strdrv(&mc_cdev, TP_CMINOR)) < 0)
		cmn_err(CE_WARN, "%s: could not unregister TP_CMINOR, err = %d", __FUNCTION__, err);
	if ((err = unregister_strdrv(&mc_cdev, MC_CMINOR)) < 0)
		cmn_err(CE_WARN, "%s: could not unregister MC_CMINOR, err = %d", __FUNCTION__, err);
	if ((err = unregister_strdrv(&mc_cdev, MGR_CMINOR)) < 0)
		cmn_err(CE_WARN, "%s: could not unregister MGR_CMINOR, err = %d", __FUNCTION__,
			err);
	if ((err = unregister_strdev(&mc_cdev, major)) < 0)
		cmn_err(CE_WARN, "%s: could not unregister driver, err = %d", __FUNCTION__, err);
	if ((err = mc_term_caches()) < 0)
		cmn_err(CE_WARN, "%s: could not terminate caches, err = %d", __FUNCTION__, err);
	return;
}

module_init(mcmuxinit);
module_exit(mcmuxexit);
