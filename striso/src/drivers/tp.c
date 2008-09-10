/*****************************************************************************

 @(#) $RCSfile: tp.c,v $ $Name:  $($Revision: 0.9.2.19 $) $Date: 2008-09-10 03:49:50 $

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

 Last Modified $Date: 2008-09-10 03:49:50 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: tp.c,v $
 Revision 0.9.2.19  2008-09-10 03:49:50  brian
 - changes to accomodate FC9, SUSE 11.0 and Ubuntu 8.04

 Revision 0.9.2.18  2008-04-29 00:02:00  brian
 - updated headers for release

 Revision 0.9.2.17  2008-04-25 08:38:30  brian
 - working up libraries modules and drivers

 Revision 0.9.2.16  2007/08/15 05:34:18  brian
 - GPLv3 updates

 Revision 0.9.2.15  2007/08/14 07:05:03  brian
 - GNUv3 header update

 Revision 0.9.2.14  2007/07/14 01:36:23  brian
 - make license explicit, add documentation

 Revision 0.9.2.13  2007/03/25 19:01:51  brian
 - changes to support 2.6.20-1.2307.fc5 kernel

 Revision 0.9.2.12  2006/10/12 10:24:50  brian
 - removed redundant debug flags, and got itot compiling

 Revision 0.9.2.11  2006/10/10 10:44:10  brian
 - updates for release, lots of additions and workup

 Revision 0.9.2.10  2006/10/03 13:52:22  brian
 - changes to pass make check target
 - added some package config.h files
 - removed AUTOCONFIG_H from Makefile.am's
 - source code changes for compile
 - added missing manual pages
 - renamed conflicting manual pages
 - parameterized include Makefile.am
 - updated release notes

 Revision 0.9.2.9  2006/07/25 06:39:18  brian
 - expanded minor device numbers and optimization and locking corrections

 Revision 0.9.2.8  2006/07/11 12:32:04  brian
 - added ISO and other implementations to distribution

 Revision 0.9.2.7  2006/05/03 01:05:55  brian
 - spelling correction

 Revision 0.9.2.6  2006/04/18 17:58:48  brian
 - added some notes

 Revision 0.9.2.5  2006/04/11 18:23:15  brian
 - working up TP driver

 Revision 0.9.2.4  2006/04/10 20:25:15  brian
 - working up TP driver

 Revision 0.9.2.3  2006/04/09 03:57:04  brian
 - working up TP driver

 Revision 0.9.2.2  2006/04/08 23:54:54  brian
 - working up ISO TP driver

 Revision 0.9.2.1  2006/04/08 03:39:15  brian
 - working up striso package

 *****************************************************************************/

#ident "@(#) $RCSfile: tp.c,v $ $Name:  $($Revision: 0.9.2.19 $) $Date: 2008-09-10 03:49:50 $"

static char const ident[] =
    "$RCSfile: tp.c,v $ $Name:  $($Revision: 0.9.2.19 $) $Date: 2008-09-10 03:49:50 $";

/*
 *  This file provides both a module and a multiplexing driver for the ISO/OSI X.224
 *  Connection-Oriented Transport Protocol.  The module can be pushed over any stream providing the
 *  Network Service Interface using Network Provider Interface (NPI) Revision 2.0.0. primitives.  An
 *  example is an X.25 Packet Layer Protocol or Frame Relay Stream.  Another good example is the RFC
 *  1006 Network Provider Interface for TCP and UDP.  Any stream providing the Network Service
 *  Interface using the Network Provider Interface (NPI) Revision 2.0.0 can also be linked underneath
 *  the multiplexing driver.
 *
 *  A single CLNS stream must be linked beneath the driver for Transport Protocol Class 4 over CLNS.
 *  Multiple CONS streams may be linked beneath the driver for Transport Protocol Classes 0, 1, 2, 3
 *  and 4 CONS for multiplexing or aggregation and separation.  The module may be pushed over a CONS
 *  stream for Transport Protocol Class 0 or Class 1, 2, 3 or 4 CONS without aggregation and
 *  separation.
 */

#include <sys/os7/compat.h>

#ifdef LINUX
#undef ASSERT

#include <linux/bitops.h>

#define t_tst_bit(nr,addr)	test_bit(nr,addr);
#define t_set_bit(nr,addr)	__set_bit(nr,addr);
#define t_clr_bit(nr,addr)	__clear_bit(nr,addr);

#include <linux/interrupt.h>

#endif				/* LINUX */

#undef socklen_t
typedef unsigned int socklen_t;

#define socklen_t socklen_t

#if defined HAVE_TIHDR_H
#   include <tidhr.h>
#else
#   include <sys/tihdr.h>
#endif

#include <sys/npi.h>
//#include <sys/npi_osi.h>

#include <sys/xti.h>

#define TP_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define TP_EXTRA	"Part of the OpenSS7 stack for Linux Fast-STREAMS"
#define TP_COPYRIGHT	"Copyright (c) 1997-2008 OpenSS7 Corporation.  All Rights Reserved."
#define TP_REVISION	"OpenSS7 $RCSfile: tp.c,v $ $Name:  $ ($Revision: 0.9.2.19 $) $Date: 2008-09-10 03:49:50 $"
#define TP_DEVICE	"SVR 4.2 STREAMS TPI OSI Transport Provider Driver"
#define TP_CONTACT	"Brian Bidulock <bidulock@opens7.org>"
#define TP_LICENSE	"GPL"
#define TP_BANNER	TP_DESCRIP	"\n" \
			TP_EXTRA	"\n" \
			TP_COPYRIGHT	"\n" \
			TP_DEVICE	"\n" \
			IP_CONTACT
#define TP_SPLASH	TP_DESCRIP	"\n" \
			TP_REVISION

#ifdef LINUX
MODULE_AUTHOR(TP_CONTACT);
MODULE_DESCRIPTION(TP_DESCRIP);
MODULE_SUPPORTED_DEVICE(TP_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(TP_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-tp");
#endif
#endif

#ifdef LFS
#define TP_DRV_ID	CONFIG_STREAMS_TP_MODID
#define TP_MOD_ID	CONFIG_STREAMS_TP_MODID
#define TP_DRV_NAME	CONFIG_STREAMS_TP_NAME
#define TP_MOD_NAME	CONFIG_STREAMS_TP_NAME
#define TP_CMAJORS	CONFIG_STREAMS_TP_NMAJOR
#define TP_CMAJOR_0	CONFIG_STREAMS_TP_MAJOR
#define TP_UNITS	CONFIG_STREAMS_TP_NMINORS
#endif				/* LFS */

#ifdef LINUX
#ifdef MODULE_ALIAS
#ifdef LFS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_TP_MODID));
MODULE_ALIAS("streams-driver-tp");
MODULE_ALIAS("streams-module-tp");
MODULE_ALIAS("streams-major-" __stringify(CONFIG_STREAMS_TP_MAJOR));
MODULE_ALIAS("/dev/streams/tp");
MODULE_ALIAS("/dev/streams/tp/*");
MODULE_ALIAS("/dev/streams/clone/tp");
#endif				/* LFS */
MODULE_ALIAS("char-major-" __stringify(TP_CMAJOR_0));
MODULE_ALIAS("char-major-" __stringify(TP_CMAJOR_0) "-*");
MODULE_ALIAS("char-major-" __stringify(TP_CMAJOR_0) "-0");
MODULE_ALIAS("char-major-" __stringify(TP_CMAJOR_0) "-" __stringify(TP_CMINOR));
MODULE_ALIAS("/dev/tp0");
MODULE_ALIAS("/dev/tp1");
MODULE_ALIAS("/dev/tp2");
MODULE_ALIAS("/dev/tp4_cons");
MODULE_ALIAS("/dev/tp4_clns");
#endif				/* MOUDLE_ALIAS */
#endif				/* LINUX */

/*
 *  STREAMS Definitions
 *  ===================
 */

#define DRV_ID		TP_DRV_ID
#define DRV_NAME	TP_DRV_NAME
#define MOD_ID		TP_MOD_ID
#define CMAJORS		TP_CMAJORS
#define CMAJOR_0	TP_CMAJOR_0
#define UNITS		TP_UNITS
#ifdef MODULE
#define DRV_BANNER	TP_BANNER
#else				/* MODULE */
#define DRV_BANNER	TP_SPLASH
#endif				/* MODULE */

STATIC struct module_info tp_minfo = {
	.mi_idnum = DRV_ID,		/* Module ID number */
	.mi_idname = DRV_NAME,		/* Module name */
	.mi_minpsz = 0,			/* Min packet size accepted */
	.mi_maxpsz = INFPSZ,		/* Max packet size accepted */
	.mi_hiwat = 1 << 15,		/* Hi water mark */
	.mi_lowat = 1 << 10,		/* Lo water mark */
};

STATIC streamscall int tp_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp);
STATIC streamscall int tp_qclose(queue_t *q, int oflags, cred_t *crp);

STATIC streamscall int tp_rput(queue_t *q, mblk_t *mp);
STATIC streamscall int tp_rsrv(queue_t *q);

STATIC struct qinit tp_rinit = {
	.qi_putp = &tp_rput,		/* Read put procedure (message from below) */
	.qi_srvp = &tp_rsrv,		/* Read service procedure */
	.qi_qopen = &tp_qopen,		/* Each open */
	.qi_qclose = &tp_qclose,	/* Last close */
	.qi_minfo = &tp_minfo,		/* Module information */
};

STATIC streamscall int tp_wput(queue_t *q, mblk_t *mp);
STATIC streamscall int tp_wsrv(queue_t *q);

STATIC struct qinit tp_winit = {
	.qi_putp = &tp_wput,		/* Write put procedure (message from above) */
	.qi_srvp = &tp_wsrv,		/* Write service procedure */
	.qi_minfo = &tp_minfo,		/* Module information */
};

STATIC struct module_info np_minfo = {
	.mi_idnum = DRV_ID,		/* Module ID number */
	.mi_idname = DRV_NAME,		/* Module name */
	.mi_minpsz = 0,			/* Min packet size accepted */
	.mi_maxpsz = INFPSZ,		/* Max packet size accepted */
	.mi_hiwat = 1 << 15,		/* Hi water mark */
	.mi_lowat = 1 << 10,		/* Lo water mark */
};

STATIC streamscall int np_rput(queue_t *q, mblk_t *mp);
STATIC streamscall int np_rsrv(queue_t *q);

STATIC struct qinit np_rinit = {
	.qi_putp = &np_rput,		/* Read put procedure (message from below) */
	.qi_srvp = &np_rsrv,		/* Read service procedure */
	.qi_minfo = &np_minfo,		/* Module information */
};

STATIC streamscall int np_wput(queue_t *q, mblk_t *mp);
STATIC streamscall int np_wsrv(queue_t *q);

STATIC struct qinit np_winit = {
	.qi_putp = &np_wput,		/* Write put procedure (message from above) */
	.qi_srvp = &np_wsrv,		/* Write service procedure */
	.qi_minfo = &np_minfo,		/* Module information */
};

STATIC struct streamtab tp_info = {
	.st_rdinit = &tp_rinit,		/* Upper read queue */
	.st_wrinit = &tp_winit,		/* Upper write queue */
	.st_muxrinit = &np_rinit,	/* Lower read queue */
	.st_muxwinit = &np_winit,	/* Lower write queue */
};

/*
 *  Options
 *  =======
 */

struct tp_options {
	unsigned long flags[3];		/* at least 96 flags */
	struct {
		t_uscalar_t xti_debug[4];
		struct t_linger xti_linger;
		t_uscalar_t xti_rcvbuf;
		t_uscalar_t xti_rcvlowat;
		t_uscalar_t xti_sndbuf;
		t_uscalar_t xti_sndlowat;
	} xti;
	struct {
		struct thrpt tco_throughput;
		struct transdel tco_transdel;
		struct rate tco_reserrorrate;
		struct rate tco_transffailprob;
		struct rate tco_estfailprob;
		struct rate tco_relfailprob;
		struct rate tco_estdelay;
		struct rate tco_reldelay;
		struct rate tco_connresil;
		t_uscalar_t tco_protection;
		t_uscalar_t tco_priority;
		t_uscalar_t tco_expd;

		t_uscalar_t tco_ltpdu;
		t_uscalar_t tco_acktime;
		t_uscalar_t tco_reastime;
		t_uscalar_t tco_prefclass;
		t_uscalar_t tco_altclass1;
		t_uscalar_t tco_altclass2;
		t_uscalar_t tco_altclass3;
		t_uscalar_t tco_altclass4;
		t_uscalar_t tco_extform;
		t_uscalar_t tco_flowctrl;
		t_uscalar_t tco_checksum;
		t_uscalar_t tco_netexp;
		t_uscalar_t tco_netrecptcf;
	} tco;
	struct {
		t_uscalar_t tcl_transdel;
		struct rate tcl_reserrorrate;
		t_uscalar_t tcl_protection;
		t_uscalar_t tcl_priority;
		t_uscalar_t tcl_checksum;
	} tcl;
};

/*
 *  Connection-mode Service
 *  -----------------------
 *  The protocol level of all subsequent options is T_ISO_TP.
 *
 *  All options are association-related, that is, they are options with end-to-end significance (see
 *  the Use of Options in XTI).  They may be negotiated in the XTI states T_IDLE and T_INCON, and
 *  are read-only in all other states except T_UNINIT.
 *
 *  Absolute Requirements
 *  ---------------------
 *  For the options in Options for Quality of Service and Expedited Data, the transport user can
 *  indicate whether the request is an absolute requirement or whether a degraded value is
 *  acceptable.  For the QOS options based on struct rate, an absolute requirement is specified via
 *  the field minacceptvalue, if that field is given a value different from T_UNSPEC.  The value
 *  specified for T_TCO_PROTECTION is an absolute requirement if the T_ABSREQ flag is set.  The
 *  values specified for T_TCO_EXPD and T_TCO_PRIORITY are never absolute requirements.
 *
 *  Further Remarks
 *  ---------------
 *  A detailed description of the options for Quality of Service can be found in ISO 8072
 *  specification. The field elements of the structures in use for the option values are
 *  self-explanatory.  Only the following details remain to be explained.
 *
 *  - If these options are returned with t_listen(), their values are related to the incoming
 *    connection and not to the transport endpoint where t_listen() was issued.  To give an example,
 *    the value of T_TCO_PROTECTION is the value sent by the calling transport user, and not the
 *    value currently effective for the endpoint (that could be retrieved by t_optmgmt() with the
 *    flag T_CURRENT set).  The option is not returned at all if the calling user did not specify
 *    it.  An analogous procedure applies for the other options.  See also The Use of Options in
 *    XTI.
 *
 *  - If, in a call to t_accept(), the called transport user tried to negotiate an optoin of higher
 *    quality than proposed, the option is rejected and the connection establishment fails (see
 *    Responding to a Negotiation Proposal).
 *
 *  - The values of the QOS options T_TCO_THROUGHPUT, T_TCO_TRANSDEL, T_TCO_RESERRRATE,
 *    T_TCO_TRANSFFAILPROB, T_TCO_ESTFAILPROB, T_TCO_RELFAILPROB, T_TCO_ESTDELAY, T_TCO_RELDELAY and
 *    T_TCO_CONNRESIL have a structured format.  A user requesting one of these options might leave
 *    a field of the structure unspecified by setting it to T_UNSPEC.  The transport provide is then
 *    free to select an appropriate value for this field.  The transport provider may return
 *    T_UNSPEC in a field of the structure to the user to indicate that it has not yet decided on a
 *    definite value for this field.
 *
 *    T_UNSPEC is not a legal value for T_TCO_PROTECTION, T_TCO_PRIORITY and T_TCO_EXPD.
 *
 *  - T_TCO_THROUGHPUT and T_TCO_TRANSDEL If avgthrpt (average throughput) is not defined (both
 *    fields set to T_UNSPEC), the transport provider considers that the average throughput has the
 *    same values as the maximum throughput (maxthrpt).  An analogous procedure applies to
 *    T_TCO_TRANSDEL.
 *
 *  - The ISO specification ISO 8073:1986 does not differentiate between average and maximum transit
 *    delay.  Transport providers that support this option adopt the values of the maximum delay as
 *    input for the CR TPDU.
 *
 *  - T_TCO_PROTECTION  This option defines the general level of protection.  The symbolic constants
 *    in the following list are used to specify the required level of protection:
 *
 *    T_NOPROTECT	- No protection feature
 *    T_PASSIVEPROTECT	- Protection against passive monitoring.
 *    T_ACITVEPROTECT	- Protection against modification, replay, addition or deletion.
 *
 *    Both flags T_PASSIVEPROTECT and T_ACTIVEPROTECT may be set simultaneously but are exclusive
 *    with T_NOPROTECT.  If the T_ACTIVEPROTECT or T_PASSIVEPROTECT flags are set, the user may
 *    indicate that this is an absolute requirement by also setting the T_ABSREQ flag.
 *
 *  - T_TCO_PRIORITY  Fiver priority levels are defined by XTI:
 *
 *    T_PRIDFLT		- Lower level.
 *    T_PRILOW		- Low level.
 *    T_PRIMID		- Mid level.
 *    T_PRIHIGH		- High level.
 *    T_PRITOP		- Top level.
 *
 *    The number of priority levels is not defined in ISO 8072:1986.  The parameter only has meaning
 *    in the context of some management entity or structure able to judge relative importance.
 *
 *  - It is recommended that transport users avoid expedited data with an ISO-over-TCP transport
 *    provider, since the RFC 1006 treatment of expedited data does not meet the data reordering
 *    requirements specified in ISO 8072:1986, and may not be supported by the provider.
 *
 *  Management Options
 *  ------------------
 *  These options are parameters of an ISO transport protocol according to ISO 8073:1986.  They are
 *  not included in the ISO transport service definition ISO 8072:1986, but are additionally offered
 *  by XTI.  Transport users wishing to be truly ISO-compliant should thus not adhere to them.
 *  T_TCO_LTPDU is the only management option supported by an ISO-over-TCP transport provider.
 *
 *  Avoid specifying both QOS parameters and management options at the same time.
 *
 *  Absolute Requirements
 *  ---------------------
 *  A request for any of these options is considered an absolute requirement.
 *
 *  Further Remarks
 *  ---------------
 *  - If these options are returned with t_listen() their values are related to the incoming
 *    connection and not to the transport endpoing where t_listen() was issued.  That means that
 *    t_optmgmt() with the flag T_CURRENT set would usually yield a different result (see the Use of
 *    Options in XTI).
 *
 *  - For management options that are subject to perr-to-peer negotiation the following holds: If,
 *    in a call to t_accept(), the called transport user tries to negotiate an option of higher
 *    quality than proposed, the option is rejected and the connection establishment fails (see
 *    Responding to a Negotiation Proposal).
 *
 *  - A connection-mode transport provider may allow  the transport user to select more than one
 *    alternative class.  The transport user may use the options T_TCO_ALLCLASS1, T_TCO_ALLCLASS2,
 *    etc. to detnote the alternatives.  A transport provider only supports an implementation
 *    dependent limit of alternatives and ignores the rest.
 *
 *  - The value T_UNSPEC is legal for all options in Management Options.  It may be set by the user
 *     to indicate that the transport provider is free to choose any appropriate value.  If returned
 *     by the transport provider, it indicates that the transport provider has not yet decided on a
 *     specific value.
 *
 *  - Legal values for the options T_TCO_PREFCLASS, T_TCO_ALTCLASS1, T_TCO_ALTCLASS2,
 *    T_TCO_ALTCLASS23 and T_TCO_ALTCLASS4 are T_CLASS0, T_CLASS1, T_CLASS2, T_CLASS3, T_CLASS4 and
 *    T_UNSPEC.
 *
 *  - If a connection has been established, T_TCO_PREFCLASS will be set to the selected value, and
 *    T_ALTCLASS1 through T_ALTCLASS4 will be set to T_UNSPEC, if these options are supported.
 *
 *  - Warning on the use of T_TCO_LTPDU: Sensible use of this option requires that the application
 *    programmer knows about system internals.  Careless setting of either a lower or a higher value
 *    than the implementation-dependent default may degrade the performance.
 *
 *    Legal values for an ISO transport provider are T_UNSPEC and multiples of 128 up to 128*(232-1)
 *    or the largest multiple of 128 that will fit in a t_uscalar_t.  Values other than powers of 2
 *    between 27 and 213 are only supported by transport providers that conform to the 1992 updated
 *    to ISO 8073.
 *
 *    Legal values for an ISO-over-TCP providers are T_UNSPEC and any power of 2 between 2**7 and
 *    2**11, and 65531.
 *
 *    The action taken by a transport provider is implementation dependent if a value is specified
 *    which is not exactly as defined in ISO 8073:1986 or its addendums.
 *
 *  - The management options are not independent of one another, and not independent of the options
 *    defined in Options for Quality of Service and Expedited Data.  A transport user must take care
 *    not to request conflicting values.  If conflicts are detected at negotiation time, the
 *    negotiation fails according to the rules for absolute requirements (see The Use of Options in
 *    XTI).  Conflicts that cannot be detected at negotiation time will lead to unpredictable
 *    results in the course of communication.  Usually, conflicts are detected at the time the
 *    connection is established.
 *
 *  Some relations that must be obeyed are:
 *
 *  - If T_TCO_EXP is set to T_YES and T_TCO_PREFCLASS is set to T_CLASS2, T_TCO_FLOWCTRL must also
 *    be set to T_YES.
 *
 *  - If T_TCO_PRECLASS is set to T_CLASS0, T_TCO_EXP must be set to T_NO.
 *
 *  - The value in T_TCO_PREFCLASS must not be lower than the value in T_TCO_ALTCLASS1,
 *    T_TCO_ALTCLASS2, and so on.
 *
 *  - Depending on the chosen QOS options, further value conflicts might occur.
 *
 *  Connectionless-mode Service
 *  ----------------------------
 */

/* Options flags */
enum {
	_T_BIT_XTI_DEBUG = 0,
	_T_BIT_XTI_LINGER,
	_T_BIT_XTI_RCVBUF,
	_T_BIT_XTI_RCVLOWAT,
	_T_BIT_XTI_SNDBUF,
	_T_BIT_XTI_SNDLOWAT,

	_T_BIT_TCO_THROUGHPUT,
	_T_BIT_TCO_TRANSDEL,
	_T_BIT_TCO_RESERRORRATE,
	_T_BIT_TCO_TRANSFFAILPROB,
	_T_BIT_TCO_ESTFAILPROB,
	_T_BIT_TCO_RELFAILPROB,
	_T_BIT_TCO_ESTDELAY,
	_T_BIT_TCO_RELDELAY,
	_T_BIT_TCO_CONNRESIL,
	_T_BIT_TCO_PROTECTION,
	_T_BIT_TCO_PRIORITY,
	_T_BIT_TCO_EXPD,

	_T_BIT_TCO_LTPDU,
	_T_BIT_TCO_ACKTIME,
	_T_BIT_TCO_REASTIME,
	_T_BIT_TCO_EXTFORM,
	_T_BIT_TCO_FLOWCTRL,
	_T_BIT_TCO_CHECKSUM,
	_T_BIT_TCO_NETEXP,
	_T_BIT_TCO_NETRECPTCF,
	_T_BIT_TCO_PREFCLASS,
	_T_BIT_TCO_ALTCLASS1,
	_T_BIT_TCO_ALTCLASS2,
	_T_BIT_TCO_ALTCLASS3,
	_T_BIT_TCO_ALTCLASS4,

	_T_BIT_TCL_TRANSDEL,
	_T_BIT_TCL_RESERRORRATE,
	_T_BIT_TCL_PROTECTION,
	_T_BIT_TCL_PRIORITY,
	_T_BIT_TCL_CHECKSUM,
	_T_BIT_LAST,
};

#define _T_FLAG_XTI_DEBUG		(1<<_T_BIT_XTI_DEBUG)
#define _T_FLAG_XTI_LINGER		(1<<_T_BIT_XTI_LINGER)
#define _T_FLAG_XTI_RCVBUF		(1<<_T_BIT_XTI_RCVBUF)
#define _T_FLAG_XTI_RCVLOWAT		(1<<_T_BIT_XTI_RCVLOWAT)
#define _T_FLAG_XTI_SNDBUF		(1<<_T_BIT_XTI_SNDBUF)
#define _T_FLAG_XTI_SNDLOWAT		(1<<_T_BIT_XTI_SNDLOWAT)

#define _T_MASK_XTI			(_T_FLAG_XTI_DEBUG \
					|_T_FLAG_XTI_LINGER \
					|_T_FLAG_XTI_RCVBUF \
					|_T_FLAG_XTI_RCVLOWAT \
					|_T_FLAG_XTI_SNDBUF \
					|_T_FLAG_XTI_SNDLOWAT)

#define _T_FLAG_TCO_THROUGHPUT		(1<<_T_BIT_TCO_THROUGHPUT)
#define _T_FLAG_TCO_TRANSDEL		(1<<_T_BIT_TCO_TRANSDEL)
#define _T_FLAG_TCO_RESERRORRATE	(1<<_T_BIT_TCO_RESERRORRATE)
#define _T_FLAG_TCO_TRANSFFAILPROB	(1<<_T_BIT_TCO_TRANSFFAILPROB)
#define _T_FLAG_TCO_ESTFAILPROB		(1<<_T_BIT_TCO_ESTFAILPROB)
#define _T_FLAG_TCO_RELFAILPROB		(1<<_T_BIT_TCO_RELFAILPROB)
#define _T_FLAG_TCO_ESTDELAY		(1<<_T_BIT_TCO_ESTDELAY)
#define _T_FLAG_TCO_RELDELAY		(1<<_T_BIT_TCO_RELDELAY)
#define _T_FLAG_TCO_CONNRESIL		(1<<_T_BIT_TCO_CONNRESIL)
#define _T_FLAG_TCO_PROTECTION		(1<<_T_BIT_TCO_PROTECTION)
#define _T_FLAG_TCO_PRIORITY		(1<<_T_BIT_TCO_PRIORITY)
#define _T_FLAG_TCO_EXPD		(1<<_T_BIT_TCO_EXPD)

#define _T_MASK_TCO_QOS			(_T_FLAG_TCO_THROUGHPUT \
					|_T_FLAG_TCO_TRANSDEL \
					|_T_FLAG_TCO_RESERRORRATE \
					|_T_FLAG_TCO_TRANSFFAILPROB \
					|_T_FLAG_TCO_ESTFAILPROB \
					|_T_FLAG_TCO_RELFAILPROB \
					|_T_FLAG_TCO_ESTDELAY \
					|_T_FLAG_TCO_RELDELAY \
					|_T_FLAG_TCO_CONNRESIL \
					|_T_FLAG_TCO_PROTECTION \
					|_T_FLAG_TCO_PRIORITY \
					|_T_FLAG_TCO_EXPD)

#define _T_FLAG_TCO_LTPDU		(1<<_T_BIT_TCO_LTPDU)
#define _T_FLAG_TCO_ACKTIME		(1<<_T_BIT_TCO_ACKTIME)
#define _T_FLAG_TCO_REASTIME		(1<<_T_BIT_TCO_REASTIME)
#define _T_FLAG_TCO_EXTFORM		(1<<_T_BIT_TCO_EXTFORM)
#define _T_FLAG_TCO_FLOWCTRL		(1<<_T_BIT_TCO_FLOWCTRL)
#define _T_FLAG_TCO_CHECKSUM		(1<<_T_BIT_TCO_CHECKSUM)
#define _T_FLAG_TCO_NETEXP		(1<<_T_BIT_TCO_NETEXP)
#define _T_FLAG_TCO_NETRECPTCF		(1<<_T_BIT_TCO_NETRECPTCF)
#define _T_FLAG_TCO_PREFCLASS		(1<<_T_BIT_TCO_PREFCLASS)
#define _T_FLAG_TCO_ALTCLASS1		(1<<_T_BIT_TCO_ALTCLASS1)
#define _T_FLAG_TCO_ALTCLASS2		(1<<_T_BIT_TCO_ALTCLASS2)
#define _T_FLAG_TCO_ALTCLASS3		(1<<_T_BIT_TCO_ALTCLASS3)
#define _T_FLAG_TCO_ALTCLASS4		(1<<_T_BIT_TCO_ALTCLASS4)

#define _T_MASK_TCO_MGMT		(_T_FLAG_TCO_LTPDU \
					|_T_FLAG_TCO_ACKTIME \
					|_T_FLAG_TCO_REASTIME \
					|_T_FLAG_TCO_EXTFORM \
					|_T_FLAG_TCO_FLOWCTRL \
					|_T_FLAG_TCO_CHECKSUM \
					|_T_FLAG_TCO_NETEXP \
					|_T_FLAG_TCO_NETRECPTCF \
					|_T_FLAG_TCO_PREFCLASS \
					|_T_FLAG_TCO_ALTCLASS1 \
					|_T_FLAG_TCO_ALTCLASS2 \
					|_T_FLAG_TCO_ALTCLASS3 \
					|_T_FLAG_TCO_ALTCLASS4)

#define _T_MASK_TCO			(_T_MASK_TCO_QOS|_T_MASK_TCO_MGMT)

#define _T_FLAG_TCL_TRANSDEL		(1<<_T_BIT_TCL_TRANSDEL)
#define _T_FLAG_TCL_RESERRORRATE	(1<<_T_BIT_TCL_RESERRORRATE)
#define _T_FLAG_TCL_PROTECTION		(1<<_T_BIT_TCL_PROTECTION)
#define _T_FLAG_TCL_PRIORITY		(1<<_T_BIT_TCL_PRIORITY)
#define _T_FLAG_TCL_CHECKSUM		(1<<_T_BIT_TCL_CHECKSUM)

#define _T_MASK_TCL			(_T_FLAG_TCL_TRANSDEL \
					|_T_FLAG_TCL_RESERRORRATE \
					|_T_FLAG_TCL_PROTECTION \
					|_T_FLAG_TCL_PRIORITY \
					|_T_FLAG_TCL_CHECKSUM)

STATIC const t_uscalar_t tp_space[_T_BIT_LAST] = {
	T_SPACE(4 * sizeof(t_uscalar_t)),
	_T_SPACE_SIZEOF(struct t_linger),
	_T_SPACE_SIZEOF(t_uscalar_t),
	_T_SPACE_SIZEOF(t_uscalar_t),
	_T_SPACE_SIZEOF(t_uscalar_t),
	_T_SPACE_SIZEOF(t_uscalar_t),
	_T_SPACE_SIZEOF(struct thrpt),
	_T_SPACE_SIZEOF(struct transdel),
	_T_SPACE_SIZEOF(struct rate),
	_T_SPACE_SIZEOF(struct rate),
	_T_SPACE_SIZEOF(struct rate),
	_T_SPACE_SIZEOF(struct rate),
	_T_SPACE_SIZEOF(struct rate),
	_T_SPACE_SIZEOF(struct rate),
	_T_SPACE_SIZEOF(struct rate),
	_T_SPACE_SIZEOF(t_uscalar_t),
	_T_SPACE_SIZEOF(t_uscalar_t),
	_T_SPACE_SIZEOF(t_uscalar_t),
	_T_SPACE_SIZEOF(t_uscalar_t),
	_T_SPACE_SIZEOF(t_uscalar_t),
	_T_SPACE_SIZEOF(t_uscalar_t),
	_T_SPACE_SIZEOF(t_uscalar_t),
	_T_SPACE_SIZEOF(t_uscalar_t),
	_T_SPACE_SIZEOF(t_uscalar_t),
	_T_SPACE_SIZEOF(t_uscalar_t),
	_T_SPACE_SIZEOF(t_uscalar_t),
	_T_SPACE_SIZEOF(t_uscalar_t),
	_T_SPACE_SIZEOF(t_uscalar_t),
	_T_SPACE_SIZEOF(t_uscalar_t),
	_T_SPACE_SIZEOF(t_uscalar_t),
	_T_SPACE_SIZEOF(t_uscalar_t),
	_T_SPACE_SIZEOF(t_uscalar_t),
	_T_SPACE_SIZEOF(struct rate),
	_T_SPACE_SIZEOF(t_uscalar_t),
	_T_SPACE_SIZEOF(t_uscalar_t),
	_T_SPACE_SIZEOF(t_uscalar_t),
};

/*
 *  Primary data structures.
 *  ========================
 */
struct tp;
struct np;
struct lp;
struct up;

struct tp_tsapid {
	uint len;			/* length of contents of buffer */
	unsigned char buf[64];		/* the buffer */
} src;

typedef struct tp {
	STR_DECLARATION (struct tp);	/* Stream declarations */
	SLIST_HEAD (lp, np);		/* list of np (link) structures */
	uchar n_prov;			/* N_CONS or N_CLNS */
	uchar class;			/* 0, 1, 2, 3 or 4 */
	uchar options;			/* class options */
	uchar addopts;			/* additional options */
	struct T_info_ack info;		/* protocol information */
	struct tp_tsapid src;		/* srce TSAP Identifier */
	struct tp_tsapid dst;		/* dest TSAP Identifier */
	struct {
		struct tp_options req;	/* request options */
		struct tp_options res;	/* response options */
	} tp_options;			/* TP options */
} tp_t;

#define TP_PRIV(__q) ((struct tp *)((__q)->q_ptr))

struct lp {
	SLIST_LINKAGE (np, lp, list);	/* linkage for list of network providers */
};

struct up {
	SLIST_LINKAGE (tp, up, list);	/* linkage for list of transport providers */
};

typedef struct np {
	STR_DECLARATION (struct np);	/* Stream declarations */
	SLIST_HEAD (up, tp);		/* list of tp (open) structures */
} np_t;

#define NP_PRIV(__q) ((struct np *)((__q)->q_ptr))

#define PRIV(__q) ((__q)->q_ptr)

typedef struct df {
	rwlock_t lock;			/* structure lock */
	SLIST_HEAD (tp, drv);		/* master list of tp (open) driver structures */
	SLIST_HEAD (tp, mod);		/* master list of tp (open) module structures */
	SLIST_HEAD (np, np);		/* master list of np (link) structures */
} df_t;

struct df master;

STATIC kmem_cachep_t tp_priv_cachep;
STATIC kmem_cachep_t tp_link_cachep;

STATIC struct tp *
tp_get(struct tp *tp)
{
	if (tp) {
		atomic_inc(&tp->refcnt);
	}
	return (tp);
}
STATIC INLINE void
tp_put(struct tp *tp)
{
	if (tp) {
		if (atomic_dec_and_test(&tp->refcnt)) {
			kmem_cache_free(tp_priv_cachep, tp);
		}
	}
}
STATIC INLINE void
tp_release(struct tp **tpp)
{
	if (tpp) {
		tp_put(*tpp);
		*tpp = NULL;
	}
}
STATIC INLINE struct tp *
tp_alloc(void)
{
	struct tp *tp;

	if ((tp = kmem_cache_alloc(tp_priv_cachep, GFP_ATOMIC))) {
		bzero(tp, sizeof(*tp));
		atomic_set(&tp->refcnt, 1);
		spin_lock_init(&tp->lock);	/* "tp-lock" */
		tp->priv_put = &tp_put;
		tp->priv_get = &tp_get;
	}
	return (tp);
};

STATIC struct np *
np_get(struct np *np)
{
	if (np) {
		atomic_inc(&np->refcnt);
	}
	return (np);
}
STATIC INLINE void
np_put(struct np *np)
{
	if (np) {
		if (atomic_dec_and_test(&np->refcnt)) {
			kmem_cache_free(tp_link_cachep, np);
		}
	}
}
STATIC INLINE void
np_release(struct np **npp)
{
	if (npp) {
		np_put(*npp);
		*npp = NULL;
	}
}
STATIC INLINE struct np *
np_alloc(void)
{
	struct np *np;

	if ((np = kmem_cache_alloc(tp_link_cachep, GFP_ATOMIC))) {
		bzero(np, sizeof(*np));
		atomic_set(&np->refcnt, 1);
		spin_lock_init(&np->lock);	/* "np-lock" */
		np->priv_put = &np_put;
		np->priv_get = &np_get;
	}
	return (np);
}

//#if defined CONFIG_STREAMS_NOIRQ || defined _TEST
#if 1

#define spin_lock_str(__lkp, __flags) \
	do { (void)__flags; spin_lock_bh(__lkp); } while (0)
#define spin_unlock_str(__lkp, __flags) \
	do { (void)__flags; spin_unlock_bh(__lkp); } while (0)

#else

#define spin_lock_str(__lkp, __flags) \
	spin_lock_irqsave(__lkp, __flags)
#define spin_unlock_str(__lkp, __flags) \
	spin_unlock_irqrestore(__lkp, __flags)

#endif

/*
 *  State Changes
 *  =============
 */

/*
 *  Upper multiplexer TPI state machine.
 *  ------------------------------------
 */
#ifdef _DEBUG
STATIC const char *
tpi_state_name(t_scalar_t state)
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
	case TS_NOSTATES:
		return ("TS_NOSTATES");
	default:
		return ("(unknown)");
	}
}
#endif

STATIC INLINE fastcall void
tpi_set_state(struct tp *tp, t_scalar_t state)
{
	printd(("%s: %p: %s <- %s\n", DRV_NAME, tp, tpi_state_name(state),
		tpi_state_name(tp->i_state)));
	tp->i_state = state;
}

STATIC INLINE fastcall t_scalar_t
tpi_get_state(struct tp *tp)
{
	return (tp->i_state);
}

STATIC INLINE fastcall t_scalar_t
tpi_chk_state(struct tp *tp, t_scalar_t mask)
{
	return (((1 << tp->i_state) & (mask)) != 0);
}

STATIC INLINE fastcall t_scalar_t
tpi_not_state(struct tp *tp, t_scalar_t mask)
{
	return (((1 << tp->i_state) & (mask)) == 0);
}

/*
 *  Lower multiplexer NPI state machine.
 *  ------------------------------------
 */
#ifdef DEBUG
STATIC const char *
npi_state_name(np_ulong state)
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
	case NS_NOSTATES:
		return ("NS_NOSTATES");
	default:
		return ("(unknown)");
	}
}
#endif

/* For the lower multiplex. */
STATIC INLINE fastcall void
npi_set_state(struct np *np, np_ulong state)
{
	printd(("%s: %p: %s <- %s\n", DRV_NAME, np, npi_state_name(state),
		npi_state_name(np->i_state)));
	np->i_state = state;
}

STATIC INLINE fastcall np_ulong
npi_get_state(struct np *np)
{
	return (np->i_state);
}

STATIC INLINE fastcall np_ulong
npi_chk_state(struct np *np, np_ulong mask)
{
	return (((1 << np->i_state) & (mask)) != 0);
}

STATIC INLINE fastcall np_ulong
npi_not_state(struct np *np, np_ulong mask)
{
	return (((1 << np->i_state) & (mask)) == 0);
}

/* For the upper multiplex. */
STATIC INLINE fastcall void
tpi_set_n_state(struct tp *tp, np_ulong state)
{
	printd(("%s: %p: %s <- %s\n", DRV_NAME, tp, npi_state_name(state),
		npi_state_name(tp->n_state)));
	tp->n_state = state;
}
STATIC INLINE fastcall np_ulong
tpi_get_n_state(struct tp *tp)
{
	return (tp->n_state);
}
STATIC INLINE fastcall np_ulong
tpi_chk_n_state(struct tp *tp, np_ulong mask)
{
	return (((q << tp->n_state) & (mask)) != 0);
}
STATIC INLINE fastcall np_ulong
tpi_not_n_state(struct tp *tp, np_ulong mask)
{
	return (((1 << tp->n_state) & (mask)) == 0);
}

/*
 *  Option Processing
 *  =================
 */
#define T_SPACE(len) \
	(sizeof(struct t_opthdr) + T_ALIGN(len))

#define T_LENGTH(len) \
	(sizeof(struct t_opthdr) + len)

#define _T_SPACE_SIZEOF(s) \
	T_SPACE(sizeof(s))

#define _T_LENGTH_SIZEOF(s) \
	T_LENGTH(sizeof(s))

STATIC int
tp_size_opts(ulong *flags)
{
	int bit = 0, size = 0;

	for (bit = 0; bit < _T_BIT_LAST; bit++)
		if (t_tst_bit(bit, flags))
			size += tp_space[bit];
	return (size);
}

/**
 * tp_size_conn_opts - wize connection indication or confirmation options
 * @tp: transport endpoint
 */
STATIC int
tp_size_conn_opts(struct tp *tp)
{
	return tp_size_opts(&tp->tp_options.req.flags | &tp->tp_options.res.flags);
}

/**
 * t_build_conn_con_opts - Build connection confirmation options
 * @tp: transport endpoint
 * @rem: remote options (always provided)
 * @loc: local options (only if this is a confirmation)
 * @op: output buffer pointer
 * @olen: output buffer len
 *
 * These are options with end-to-end significance plus any options without end-to-end significance
 * that were requested for negotiation in the connection request.  For a connection indication, this
 * is only options with end-to-end significance.  For this to work with connection indications, all
 * options request flags must be set to zero.  The return value is the resulting size of the options
 * buffer, or a negative error number on software fault.
 *
 * The t_connect() or t_rcvconnect() functions return the values of all options with end-to-end
 * significance that were received with the connection response and the negotiated values of those
 * options without end-to-end significance that had been specified on input.  However, options
 * specified on input with t_connect() call that are not supported or refer to an unknown option
 * level are discarded and not returned on output.
 *
 * The status field of each option returned with t_connect() or t_rcvconnect() indicates if the
 * proposed value (T_SUCCESS) or a degraded value (T_PARTSUCCESS) has been negotiated.  The status
 * field of received ancillary information (for example, T_IP options) that is not subject to
 * negotiation is always set to T_SUCCESS.
 *
 * If this is a connection indication, @rem is the options passed by the remote end.
 */
STATIC int
t_build_conn_opts(struct tp *tp, struct tp_options *rem, struct tp_options *rsp, unsigned char *op,
		  size_t olen, int indication)
{
	struct t_opthdr *oh;
	unsigned long flags[4], toggles[4];

	if (op == NULL || olen == 0)
		return (0);
	if (!tp)
		goto eproto;
	{
		int i;

		for (i = 0; i < 4; i++) {
			flags[i] = rem->flags[i] | rsp->flags[i];
			toggles[i] = rem->flags[i] ^ rsp->flags[i];
		}
	}
	oh = _T_OPT_FIRSTHDR_OFS(op, olen, 0);
	if (t_tst_bit(_T_BIT_XTI_DEBUG, flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(rem->xti.xti_debug);
		oh->level = XTI_GENERIC;
		oh->name = XTI_DEBUG;
		oh->status = T_SUCCESS;
		/* No end-to-end significance */
		/* FIXME: range check parameter */
		bcopy(rem->xti.xti_debug, T_OPT_DATA(oh), sizeof(rem->xti.xti_debug));
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_XTI_LINGER, flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(rem->xti.xti_linger);
		oh->level = XTI_GENERIC;
		oh->name = XTI_LINGER;
		oh->status = T_SUCCESS;
		/* No end-to-end significance */
		/* FIXME: range check parameter */
		*((struct t_linger *) T_OPT_DATA(oh)) = rem->xti.xti_linger;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_XTI_RCVBUF, flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(rem->xti.xti_rcvbuf);
		oh->level = XTI_GENERIC;
		oh->name = XTI_RCVBUF;
		oh->status = T_SUCCESS;
		/* No end-to-end significance */
		/* FIXME: range check parameter */
		*((t_uscalar_t *) T_OPT_DATA(oh)) = rem->xti.xti_rcvbuf;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_XTI_RCVLOWAT, flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(rem->xti.xti_rcvlowat);
		oh->level = XTI_GENERIC;
		oh->name = XTI_RCVLOWAT;
		oh->status = T_SUCCESS;
		/* No end-to-end significance */
		/* FIXME: range check parameter */
		*((t_uscalar_t *) T_OPT_DATA(oh)) = rem->xti.xti_rcvlowat;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_XTI_SNDBUF, flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(rem->xti.xti_sndbuf);
		oh->level = XTI_GENERIC;
		oh->name = XTI_SNDBUF;
		oh->status = T_SUCCESS;
		/* No end-to-end significance */
		/* FIXME: range check parameter */
		*((t_uscalar_t *) T_OPT_DATA(oh)) = rem->xti.xti_sndbuf;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_XTI_SNDLOWAT, flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(rem->xti.xti_sndlowat);
		oh->level = XTI_GENERIC;
		oh->name = XTI_SNDLOWAT;
		oh->status = T_SUCCESS;
		/* No end-to-end significance */
		/* FIXME: range check parameter */
		*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.xti.xti_sndlowat;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	switch (tp->service) {
	case T_COTS:
		if (t_tst_bit(_T_BIT_TCO_THROUGHPUT, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.tco_throughput);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_THROUGHPUT;
			oh->status = T_SUCCESS;
			/* End-to-end significance. */
			/* Check requested but not responded, responded without request */
			if (t_bit_tst(_T_BIT_TCO_THROUGHPUT, toggled))
				oh->status = T_FAILURE;
			/* Check if we got downgraded. */
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.tco_throughput)) T_OPT_DATA(oh)) =
			    rem->tco.tco_throughput;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_TRANSDEL, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.tco_transdel);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_TRANSDEL;
			oh->status = T_SUCCESS;
			if (t_bit_tst(_T_BIT_TCO_TRANSDEL, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.tco_transdel)) T_OPT_DATA(oh)) = rem->tco.tco_transdel;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_RESERRORRATE, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.tco_reserrorrate);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_RESERRORRATE;
			oh->status = T_SUCCESS;
			if (t_bit_tst(_T_BIT_TCO_RESERRORRATE, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.tco_reserrorrate)) T_OPT_DATA(oh)) =
			    rem->tco.tco_reserrorrate;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_TRANSFFAILPROB, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.tco_transffailprob);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_TRANSFFAILPROB;
			oh->status = T_SUCCESS;
			if (t_bit_tst(_T_BIT_TCO_TRANSFFAILPROB, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.tco_trasffailprob)) T_OPT_DATA(oh)) =
			    rem->tco.tco_trasffailprob;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_ESTFAILPROB, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.tco_estfailprob);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_ESTFAILPROB;
			oh->status = T_SUCCESS;
			if (t_bit_tst(_T_BIT_TCO_ESTFAILPROB, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.tco_estfailprob)) T_OPT_DATA(oh)) =
			    rem->tco.tco_estfailprob;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_RELFAILPROB, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.tco_relfailprob);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_RELFAILPROB;
			oh->status = T_SUCCESS;
			if (t_bit_tst(_T_BIT_TCO_RELFAILPROB, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.tco_relfailprob)) T_OPT_DATA(oh)) =
			    rem->tco.tco_relfailprob;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_ESTDELAY, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.tco_estdelay);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_ESTDELAY;
			oh->status = T_SUCCESS;
			if (t_bit_tst(_T_BIT_TCO_ESTDELAY, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.tco_estdelay)) T_OPT_DATA(oh)) = rem->tco.tco_estdelay;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_RELDELAY, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.tco_reldelay);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_RELDELAY;
			oh->status = T_SUCCESS;
			if (t_bit_tst(_T_BIT_TCO_RELDELAY, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.tco_reldelay)) T_OPT_DATA(oh)) = rem->tco.tco_reldelay;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_CONNRESIL, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.tco_connresil);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_CONNRESIL;
			oh->status = T_SUCCESS;
			if (t_bit_tst(_T_BIT_TCO_CONNRESIL, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.tco_connresil)) T_OPT_DATA(oh)) =
			    rem->tco.tco_connresil;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_PROTECTION, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.tco_protection);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_PROTECTION;
			oh->status = T_SUCCESS;
			if (t_bit_tst(_T_BIT_TCO_PROTECTION, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.tco_protection)) T_OPT_DATA(oh)) =
			    rem->tco.tco_protection;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_PRIORITY, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.tco_priority);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_PRIORITY;
			oh->status = T_SUCCESS;
			if (t_bit_tst(_T_BIT_TCO_PRIORITY, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.tco_priority)) T_OPT_DATA(oh)) = rem->tco.tco_priority;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_EXPD, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.tco_expd);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_EXPD;
			oh->status = T_SUCCESS;
			if (t_bit_tst(_T_BIT_TCO_EXPD, toggled))
				oh->status = T_FAILURE;
			if (rem->tco.tco_expd != T_UNSPEC) {
			}
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.tco_expd)) T_OPT_DATA(oh)) = rem->tco.tco_expd;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_LTPDU, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.tco_ltpdu);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_LTPDU;
			oh->status = T_SUCCESS;
			if (t_bit_tst(_T_BIT_TCO_LTPDU, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.tco_ltpdu)) T_OPT_DATA(oh)) = rem->tco.tco_ltpdu;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_ACKTIME, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.tco_acktime);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_ACKTIME;
			oh->status = T_SUCCESS;
			if (t_bit_tst(_T_BIT_TCO_ACKTIME, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.tco_acktime)) T_OPT_DATA(oh)) = rem->tco.tco_acktime;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_REASTIME, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.tco_reastime);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_REASTIME;
			oh->status = T_SUCCESS;
			if (t_bit_tst(_T_BIT_TCO_REASTIME, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.tco_reastime)) T_OPT_DATA(oh)) = rem->tco.tco_reastime;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_EXTFORM, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.tco_extform);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_EXTFORM;
			oh->status = T_SUCCESS;
			if (t_bit_tst(_T_BIT_TCO_EXTFORM, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.tco_extform)) T_OPT_DATA(oh)) = rem->tco.tco_extform;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_FLOWCTRL, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.tco_flowctrl);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_FLOWCTRL;
			oh->status = T_SUCCESS;
			if (t_bit_tst(_T_BIT_TCO_FLOWCTRL, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.tco_flowctrl)) T_OPT_DATA(oh)) = rem->tco.tco_flowctrl;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_CHECKSUM, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.tco_checksum);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_CHECKSUM;
			oh->status = T_SUCCESS;
			if (t_bit_tst(_T_BIT_TCO_CHECKSUM, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.tco_checksum)) T_OPT_DATA(oh)) = rem->tco.tco_checksum;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_NETEXP, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.tco_netexp);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_NETEXP;
			oh->status = T_SUCCESS;
			if (t_bit_tst(_T_BIT_TCO_NETEXP, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.tco_netexp)) T_OPT_DATA(oh)) = rem->tco.tco_netexp;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_NETRECPTCF, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.tco_netrecptcf);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_NETRECPTCF;
			oh->status = T_SUCCESS;
			if (t_bit_tst(_T_BIT_TCO_NETRECPTCF, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.tco_netrecptcf)) T_OPT_DATA(oh)) =
			    rem->tco.tco_netrecptcf;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_PREFCLASS, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.tco_prefclass);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_PREFCLASS;
			oh->status = T_SUCCESS;
			if (t_bit_tst(_T_BIT_TCO_PREFCLASS, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.tco_prefclass)) T_OPT_DATA(oh)) =
			    rem->tco.tco_prefclass;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_ALTCLASS1, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.tco_altclass1);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_ALTCLASS1;
			oh->status = T_SUCCESS;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.tco_altclass1)) T_OPT_DATA(oh)) =
			    rem->tco.tco_altclass1;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_ALTCLASS2, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.tco_altclass2);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_ALTCLASS2;
			oh->status = T_SUCCESS;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.tco_altclass2)) T_OPT_DATA(oh)) =
			    rem->tco.tco_altclass2;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_ALTCLASS3, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.tco_altclass3);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_ALTCLASS3;
			oh->status = T_SUCCESS;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.tco_altclass3)) T_OPT_DATA(oh)) =
			    rem->tco.tco_altclass3;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_ALTCLASS4, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.tco_altclass4);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_ALTCLASS4;
			oh->status = T_SUCCESS;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.tco_altclass4)) T_OPT_DATA(oh)) =
			    rem->tco.tco_altclass4;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		break;
	case T_CLTS:
		if (t_tst_bit(_T_BIT_TCL_TRANSDEL, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tcl.tcl_transdel);
			oh->level = T_ISO_TP;
			oh->name = T_TCL_TRANSDEL;
			oh->status = T_SUCCESS;
			if (t_bit_tst(_T_BIT_TCL_TRANSDEL, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tcl.tcl_transdel)) T_OPT_DATA(oh)) = rem->tcl.tcl_transdel;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCL_RESERRORRATE, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tcl.tcl_reserrorrate);
			oh->level = T_ISO_TP;
			oh->name = T_TCL_RESERRORRATE;
			oh->status = T_SUCCESS;
			if (t_bit_tst(_T_BIT_TCL_RESERRORRATE, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tcl.tcl_reserrorrate)) T_OPT_DATA(oh)) =
			    rem->tcl.tcl_reserrorrate;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCL_PROTECTION, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tcl.tcl_protection);
			oh->level = T_ISO_TP;
			oh->name = T_TCL_PROTECTION;
			oh->status = T_SUCCESS;
			if (t_bit_tst(_T_BIT_TCL_PROTECTION, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tcl.tcl_protection)) T_OPT_DATA(oh)) =
			    rem->tcl.tcl_protection;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCL_PRIORITY, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tcl.tcl_priority);
			oh->level = T_ISO_TP;
			oh->name = T_TCL_PRIORITY;
			oh->status = T_SUCCESS;
			if (t_bit_tst(_T_BIT_TCL_PRIORITY, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tcl.tcl_priority)) T_OPT_DATA(oh)) = rem->tcl.tcl_priority;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCL_CHECKSUM, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tcl.tcl_checksum);
			oh->level = T_ISO_TP;
			oh->name = T_TCL_CHECKSUM;
			oh->status = T_SUCCESS;
			if (t_bit_tst(_T_BIT_TCL_CHECKSUM, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tcl.tcl_checksum)) T_OPT_DATA(oh)) = rem->tcl.tcl_checksum;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		break;
	}
	/* expect oh to be NULL (filled buffer) */
	assure(oh == NULL);
	return (olen);
	// return ((unsigned char *) oh - op); /* return actual length */
      eproto:
	swerr();
	return (-EPROTO);
      efault:
	swerr();
	return (-EFAULT);
}

/*
 *  Parse connection request or response options.
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Only legal options can be negotiated; illegal options cause failure.  An option is illegal if
 *  the following applies: 1) the length specified in t_opthdr.len exceeds the remaining size of the
 *  option buffer (counted from the beginning of the option); 2) the option value is illegal: the
 *  legal values are defined for each option.  If an illegal option is passed to XTI, the following
 *  will happen: ... if an illegal option is passed to t_accept() or t_connect() then either the
 *  function failes with t_errno set to [TBADOPT] or the connection establishment fails at a later
 *  stage, depending on when the implementation detects the illegal option. ...
 *
 *  If the tansport user passes multiple optiohs in one call and one of them is illegal, the call
 *  fails as described above.  It is, however, possible that some or even all of the smbmitted legal
 *  options were successfully negotiated.  The transport user can check the current status by a call
 *  to t_optmgmt() with the T_CURRENT flag set.
 *
 *  Specifying an option level unknown to the transport provider does not fail in calls to
 *  t_connect(), t_accept() or t_sndudata(); the option is discarded in these cases.  The function
 *  t_optmgmt() fails with [TBADOPT].
 *
 *  Specifying an option name that is unknown to or not supported by the protocol selected by the
 *  option level does not cause failure.  The option is discarded in calles to t_connect(),
 *  t_accept() or t_sndudata().  The function t_optmgmt() returns T_NOTSUPPORT in the status field
 *  of the option.
 *
 *  If a transport user requests negotiation of a read-only option, or a non-privileged user
 *  requests illegal access to a privileged option, the following outcomes are possible: ... 2) if
 *  negotiation of a read-only option is required, t_accept() or t_connect() either fail with
 *  [TACCES], or the connection establishment aborts and a T_DISCONNECT event occurs.  If the
 *  connection aborts, a synchronous call to t_connect() failes with [TLOOK].  It depdends on timing
 *  an implementation conditions whether a t_accept() call still succeeds or failes with [TLOOK].
 *  If a privileged option is illegally requested, the option is quietly ignored.  (A non-privileged
 *  user shall not be able to select an option which is privileged or unsupported.)
 *
 *  If multiple options are submitted to t_connect(), t_accept() or t_sndudata() and a read-only
 *  option is rejected, the connection or the datagram transmission fails as described.  Options
 *  that could be successfully negotiated before the erroneous option was processed retain their
 *  negotiated values.  There is no roll-back mechanmism.
 */
STATIC int
t_parse_conn_opts(struct tp *tp, const uchar *ip, size_t ilen, int request)
{
	const struct t_opthdr *ih;

	/* clear flags, these flags will be used when sending a connection confirmation to
	   determine which options to include in the confirmstion. */
	bzero(tp->tp_options.flags, sizeof(tp->tp_options.flags));
	if (ip == NULL || ilen == 0)
		return (0);
	if (!ss || !ss->sock || !(sk = ss->sock->sk))
		goto eproto;
	/* For each option recognized, we test the requested value for legallity, and then set the
	   requested value in the stream's option buffer and mark the option requested in the
	   options flags.  If it is a request (and not a response), we negotiate the value to the
	   underlying.  socket.  Once the protocol has completed remote negotiation, we will
	   determine whether the negotiation was successful or partially successful.  See
	   t_build_conn_opts(). */
	/* For connection responses, test the legality of each option and mark the option in the
	   options flags.  We do not negotiate to the socket because the final socket is not
	   present.  t_set_options() will read the flags and negotiate to the final socket after
	   the connection has been accepted. */
	for (ih = _T_OPT_FIRSTHDR_OFS(ip, ilen, 0); ih; ih = _T_OPT_NEXTHDR_OFS(ip, ilen, ih, 0)) {
	}
}

/*
 *  NS-User Primitives sent downstream.
 *  -----------------------------------
 */

/**
 * ne_unitdata_req - generate an NE_UNITDATA_REQ event
 * @q: invoking queue
 * @dp: data block containing user data
 */
STATIC INLINE fastcall __hot_put int
ne_unitdata_req(queue_t *q, mblk_t *dp)
{
}

/**
 * ne_data_req - generate an NE_DATA_REQ event
 * @q: invoking queue
 * @dp: data block containing user data
 */
STATIC INLINE fastcall __hot_put int
ne_data_req(queue_t *q, mblk_t *dp)
{
}

/**
 * ne_conn_req - generate an NE_CONN_REQ event
 * @q: invoking queue
 * @dp: data block containing user data
 */
STATIC int
ne_conn_req(queue_t *q, mblk_t *dp)
{
}

/**
 * ne_conn_res - generate an NE_CONN_RES event
 * @q: invoking queue
 * @dp: data block containing user data
 */
STATIC int
ne_conn_res(queue_t *q, mblk_t *dp)
{
}

/**
 * ne_discon_req - generate an NE_DISCON_REQ event
 * @q: invoking queue
 * @dp: data block containing user data
 */
STATIC int
ne_discon_req(queue_t *q, mblk_t *dp)
{
}

/**
 * ne_exdata_req - generate an NE_EXDATA_REQ event
 * @q: invoking queue
 * @dp: data block containing user data
 */
STATIC INLINE fastcall __hot_put int
ne_exdata_req(queue_t *q, mblk_t *dp)
{
}

/**
 * ne_datack_req - generate an NE_DATACK_REQ event
 * @q: invoking queue
 * @dp: data block containing user data
 */
STATIC INLINE fastcall __hot_put int
ne_datack_req(queue_t *q, mblk_t *dp)
{
}

/**
 * ne_reset_req - generate an NE_RESET_REQ event
 * @q: invoking queue
 * @dp: data block containing user data
 */
STATIC int
ne_reset_req(queue_t *q)
{
}

/**
 * ne_reset_res - generate an NE_RESET_RES event
 * @q: invoking queue
 * @dp: data block containing user data
 */
STATIC int
ne_reset_res(queue_t *q)
{
}

/**
 * ne_info_req - generate an NE_INFO_REQ event
 * @q: invoking queue (lower mux write queue)
 *
 * Note that if the buffer allocation fails, a bufcall will be generated against @q.  @q's service
 * procedure (wakeup function) should check and if the state still calls for the generation of an
 * information request, recall this function.
 */
STATIC int
ne_info_req(queue_t *q)
{
	N_info_req_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (N_info_req_t *) mp->b_wptr;
		p->PRIM_type = N_INFO_REQ;
		mp->b_wptr += sizeof(*p);
		putnext(q, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * ne_bind_req - generate an NE_BIND_REQ event
 * @q: invoking queue
 * @dp: data block containing user data
 */
STATIC int
ne_bind_req(queue_t *q)
{
}

/**
 * ne_unbind_req - generate an NE_UNBIND_REQ event
 * @q: invoking queue
 * @dp: data block containing user data
 */
STATIC int
ne_unbind_req(queue_t *q)
{
}

/**
 * ne_optmgmt_req - generate an NE_OPTMGMT_REQ event
 * @q: invoking queue
 * @dp: data block containing user data
 */
STATIC int
ne_optmgmt_req(queue_t *q)
{
}

/*
 *  Sending Transport Protocol messages.
 *  ------------------------------------
 */

/* Class options */
#define _T_F_CO_RESERVED4	(1<<3)	/* reserved */
#define _T_F_CO_RESERVED3	(1<<2)	/* reserved */
#define _T_F_CO_EXTFORM		(1<<1)	/* extended format */
#define _T_F_CO_FLOWCTRL	(1<<0)	/* explicit flow control */

/* Additional options - XTI doesn't know about these first three */
#define _T_F_AO_NBLKEXPDATA	(1<<6)	/* non-blocking expedited data */
#define _T_F_AO_REQUESTACK	(1<<5)	/* request acknowledgement */
#define _T_F_AO_SELECTACK	(1<<4)	/* selective acknowledgement */
#define _T_F_AO_NETEXP		(1<<3)	/* network expedited data */
#define _T_F_AO_NETRECPTCF	(1<<2)	/* receipt confirmation */
#define _T_F_AO_CHECKSUM	(1<<1)	/* checksum */
#define _T_F_AO_EXPD		(1<<0)	/* transport expedited data */

/* these are really size = (1<<value) */
#define _T_TPDU_SIZE_8192	0x0d
#define _T_TPDU_SIZE_4096	0x0c
#define _T_TPDU_SIZE_2048	0x0b
#define _T_TPDU_SIZE_1024	0x0a
#define _T_TPDU_SIZE_512	0x09
#define _T_TPDU_SIZE_256	0x08
#define _T_TPDU_SIZE_128	0x07

/* disconnect reasons, these should be in TPI header file */
#define _TP_REASON_UNSPECIFIED		0x00
#define _TP_REASON_TSAP_CONGESTION	0x01
#define _TP_REASON_TSAP_NO_SESSION	0x02	/* persistent */
#define _TP_REASON_ADDRESS_UNKNOWN	0x03	/* persistent */
#define _TP_REASON_NORMAL_DISCONNECT	0x80
#define _TP_REASON_REMOTE_CONGESTION	0x81
#define _TP_REASON_NEGOTIATION_FAILED	0x82	/* persistent */
#define _TP_REASON_DUPLICATE_SREF	0x83
#define _TP_REASON_MISMATCH_REFS	0x84
#define _TP_REASON_PROTOCOL_ERROR	0x85
#define _TP_REASON_NOT_USED1		0x86
#define _TP_REASON_REFERENCE_OVERFLOW	0x87
#define _TP_REASON_REFUSED		0x88
#define _TP_REASON_NOT_USED2		0x89
#define _TP_REASON_INVALID_LENGTH	0x8a

/* error causes, these should be in TPI header file */
#define _TP_ERROR_UNSPECIFIED		0x00
#define _TP_ERROR_INVALID_PARM_TYPE	0x01
#define _TP_ERROR_INVALID_TPDU_TYPE	0x02
#define _TP_ERROR_INVALID_PARM_VALUE	0x03

/* message types */
#define	_TP_MT_ED	0x10
#define	_TP_MT_EA	0x20
#define _TP_MT_UD	0x40
#define	_TP_MT_RJ	0x50
#define	_TP_MT_AK	0x60
#define	_TP_MT_ER	0x70
#define	_TP_MT_DR	0x80
#define	_TP_MT_DC	0xc0
#define	_TP_MT_CC	0xd0
#define	_TP_MT_CR	0xe0
#define	_TP_MT_DT	0xf0

#define _TP_PT_INVALID_TPDU	0xc1	/* invalid TPDU */

#define _TP_PT_TPDU_SIZE	0xc0	/* TPDU size */
#define _TP_PT_CGTRANSSEL	0xc1	/* calling transport selector */
#define _TP_PT_CDTRANSSEL	0xc2	/* called transport selector */
#define _TP_PT_CHECKSUM		0xc3	/* checksum */
#define _TP_PT_VERSION		0xc4	/* version number */
#define _TP_PT_PROTECTION	0xc5	/* protection */
#define _TP_PT_ADDOPTIONS	0xc6	/* additional options */
#define _TP_PT_ALTCLASS		0xc7	/* alternative protocol classes */

#define _TP_PT_ACKTIME		0x85	/* acknowledgement time */
#define _TP_PT_RESERRORRATE	0x86	/* residual error rate */
#define _TP_PT_PRIORITY		0x87	/* priority */
#define _TP_PT_TRANSDEL		0x88	/* transit delay */
#define _TP_PT_THROUGHPUT	0x89	/* throughput */
#define _TP_PT_SUBSEQUENCE	0x8a	/* subsequence number */
#define _TP_PT_REASTIME		0x8b	/* reassignment time */
#define _TP_PT_FLOWCTLCF	0x8c	/* flow control confirmation */
#define _TP_PT_SELECTACK	0x8f	/* selective acknowledgement */

#define _TP_PT_ED_TPDU_NR	0x90	/* ED-TPDU-NR */

#define _TP_PT_DIAGNOSTIC	0xe0	/* diagnostic */

#define _TP_PT_PREF_TPDU_SIZE	0xf0	/* preferred TPDU size */
#define _TP_PT_INACTTIME	0xf2	/* inactivity time */

/**
 * tp_add_checksum - complete the checksum
 * @mp: message containing TPDU
 *
 * Add a checksum parameter to the variable part and calculate a checksum for the resulting TPDU.
 * This calculation follows the procedures in X.224 Annex B and X.234 Annex C.
 */
STATIC INLINE __hot void
tp_add_checksum(mblk_t *mp)
{
	int n;
	mblk_t *bp;
	int c0 = 0, c1 = 0, L = 0;

	mp->b_rptr[0] += 4;	/* add 4 to length indicator */
	*mp->b_wptr++ = _TP_PT_CHECKSUM;
	*mp->b_wptr++ = 2;
	n = mp->b_wptr - mp->b_rptr;
	/* zero checksum to begin with */
	*mp->b_wptr++ = 0;
	*mp->b_wptr++ = 0;

	for (bp = mp; bp; bp = bp->b_cont) {
		register unsigned char *p = bp->b_rptr;
		register unsigned char *e = bp->b_wptr;

		L += e - p;
		while (p < e) {
			c0 += *p++;
			c1 += c0;
		}
	}

	mp->b_rptr[n] = -c1 + (L - n) * c0;
	mp->b_rptr[n + 1] = c1 - (L - n + 1) * co;
	return;
}

/**
 * tp_pack_cr - create a Connection Request (CR) TPDU
 * @tp: sending transport endpoint
 * @q: invoking queue
 *
 * CDT
 * DST-REF: (set to zero)
 * SRC-REF: (set to assigned source reference)
 * CLASS and OPTIONS:
 * Calling Transport Selector:
 * Called Transport Selector:
 * TPDU size (proposed)
 * preferred maximum TPDU size (proposed)
 * version number
 * protection parameter
 * checksum
 * additional options depending on class
 * alternative protocol class(es)
 * acknowledgement time
 * inactivity time
 * throughput (proposed)
 * residual error rate (proposed)
 * priority (proposed)
 * transit delay (proposed)
 * reassignment time
 * user data
 *
 * A transport connection is established by means of one transport entity (the
 * initiator) transmitting a CR-TPDU to the other transport entity (the
 * responder), which replies with a CC-TPDU.
 *
 * Before sneding the CR-TPDU, the intiator assigns the transport connection
 * being created to one (or more if the splitting procedure is being used)
 * network connection(s).  It is this set of network connections over which the
 * TPDUs are sent.
 *
 *   NOTE 1 - Even if the initiator assigns the transport connection to more
 *   than one network connection, all CR-TPDUs (if repeated) or DR-TPDUs with
 *   DST-REF set to zero which are sent prior to the receipt of the CC-TPDU
 *   shall be sent on the same network connection, unless an N-DISCONNECT
 *   indication is received.  (This is necessary because the remote entity may
 *   not support class 4 and therefore may not recognize splitting.)  If the
 *   initiator has made other assignments, it will use them only after receive
 *   of a class 4 CC-TPDU (see also the splitting procedure 6.23).
 *
 * During this exchange, all information and parameters need for the transport
 * entities to operate shall be exchanged or negotiated.
 *
 *   NOTE 2 - Except in class 4, it is recommended that the initiator start an
 *   optional timer TS1 at the time the CR-TPDU is sent.  This timer should be
 *   stopped when the connection is considered as accepted or refused or
 *   unsuccessful.  If the timer expires, the intiator should reset or
 *   disconnect the network connection, and in classes 1 and 3, freeze the
 *   reference (see 6.18).  For all other transport connection(s) multiplexed
 *   on the same network connection, the procedures for reset or disconnect as
 *   appropriate should be followed.
 *
 * ...
 *
 * The following information is exchanged:
 *
 * a) References - Each transport entity chooses a reference to be used by the
 *    peer entity which is 16-bits long and which is arbitrary under the
 *    following restrictions:
 *
 *    1) it shall not already be in use nor frozen (see 6.18);
 *    2) it shall not be zero.
 *
 *    This mechanism is symmetrical and provide identification of the transport
 *    connection independent of the network connection.  The range of
 *    references used for transport connections, in a given transport entity,
 *    is a local matter.
 *
 * b) Calling, Called and Responding Transport-Selectors (optional) - When
 *    either network address unambiguously defines the transport address, this
 *    information may be omitted.
 *
 * c) Initial credit - Only relevant for classes which include the explicit
 *    flow control function.
 *
 * d) User data - Not available if class 0 is the preferred class (see Note 3).
 *    Up to 32 octets in other classes.
 *
 *    NOTE 3 - If class 0 is a valid response according to Table 3, inclusing
 *    of user data in the CR-TPDU may cause the responding entity to refuse the
 *    connection (for example, if it only supports class 0).
 *
 * e) Acknowledgement time - Only in class 4.
 *
 * f) Checksum paraemter - Only in class 4.
 *
 * g) Protection parameter - This parameter and its semantics are user defined.
 *
 * h) Inactivity time - Only in class 4.  The inactivity time parameter shall
 *    not be included in a CC-TPDU if it was not present in the corresponding
 *    CR-TPDU.
 *
 * The following negotiations take place:
 *
 * i) The initiator shall propose a preferred class and may propose any number
 *    of alternative classes which permit a valid response as defined in Table
 *    3.  The initiator should assume when it sends the CR-TPDU that its
 *    preferred class will be agreed to, and commence the procedures associated
 *    with that class, except that if class 0 or class 1 is an alternative
 *    class, multiplexing shall not commence until a CC-TPDU selecting the use
 *    of classes 2, 3 or 4 has been received.
 *
 *    NOTE 4 - This means, for example, that when the preferred class includes
 *    resynchronization (see 6.14) the resynchronization will occur if a reset
 *    is signalled during connection establishment.
 *
 *    The responder shall select one class defined in Table 3 as a valid
 *    response corresponding to the preferred class and to the class(es), if
 *    any, contained in the alternative class parameter of the CR-TPDU.  It
 *    shall indicate the selected class in the CC-TPDU and shall follow the
 *    procedures for the selected class.
 *
 *    If the preferred class is not selected, then on receipt of the CC-TPDU,
 *    the initiator shall adjust its operation according to the procedures of
 *    the selected class.
 *
 *    NOTE 5 - The valid responses indicated in Table 3 result from both
 *    explicit negotiation, whereby each of the classes proposed is a valid
 *    response, and implement negotiation whereby:
 *    - if class 3 or 4 is proposed, then class 2 is a valid response.
 *    - if class 1 is proposed, then class 0 is a valid response.
 *
 *    NOTE 6 - Negotiation from class 2 to class 1 and from any class to a
 *    higher-numbered class is not valid.
 *
 *    NOTE 7 - Redundant combinations are not a protocol error.
 *
 * j) TPDU size - The initiator may propose a maimum size for TPDUs, and the
 *    responder may accept this value or respond with any value between 128 and
 *    the proposed value in the set of values available [see 13.3.4 b)].
 *
 *    NOTE 8 - The length of the CR-TPDU does not exceed 128 octets (see 13.3).
 *    NOTE 9 - The transport entities may have knowledge, by some local means,
 *    of the maximum available NSDU size.
 *
 * k) Preferred maximum TPDU size - The value of this parameter, multiplied by
 *    128, yields the proposed or accepted maximum TPDU size in octets.  The
 *    initiator may propose a preferred maximum size fo TPDUs and the responder
 *    may accept this value or repsond with a smaller value.
 *
 *    NOTE 10 - If this parameter is used in a CR-TPDU without also including
 *    the TPDU size parameter, this will result in a maximum TPDU size of 128
 *    octets being selected if the remote entity does not recognize the
 *    preferred TPDU size parameter.  Therefore, it is reocmmended that both
 *    parameters be included in the CR-TPDU.
 *
 *    If the preferred maximum TPDU size parameter is present in a CR-TPDU the
 *    responder shall either:
 *
 *    - ignore the preferred maximum TPDU size parameter and follow TPDU size
 *      negotiation as defined in 6.5.4 j); or
 *
 *    - use the preferrend maximum TPDU size parameter to determine the maximum
 *      TPDU size requested by the initiator and ignore the TPDU size
 *      parameter.  In this case the responder shall use the preferred maximum
 *      TPDU size parameter in the CC-TPDU and shall not include the TPDU size
 *      parameter in the CC-TPDU.
 *
 *    If the preferred maximum TPDU size parameter is not present in the
 *    CR-TPDU it shall not be included in the corresponding CC-TPDU.  In this
 *    case TPDU size negotiation is as defined in 6.5.4 j).
 *
 * l) Normal or extended format - Either normal or extended format is
 *    available.  When extended is used, this applies to CDT, TPDU-NR,
 *    ED-TPDU-NR, YR-TU-NR and YR-EDTU-NR parameters.
 *
 * m) Checksum selection - This defines whether or not TPDUs of the connection
 *    are to include a checksum.
 *
 * n) Quality of service parameters - This defines the throughput, transit
 *    delay, priority and residual error rate.
 *
 *    NOTE 11 - The transport service defines transit delay as requiring a
 *    previously stated average TSDU size as a basis for any specification.
 *    This protocol as speciied in 13.3.4 m), uses a value at 128 octets.
 *    Conversion to and from specifications based upon some other value is a
 *    local matter.
 *
 * o) The non-use of explicit flow control in class 2.
 *
 * p) The use of network receipt confirmation and network expedited when class
 *    1 is to be used.
 *
 * q) Use of expedited data transfer service - This allows both TS-users to
 *    negotiate the use or non-use of the expedite data transport service as
 *    defined in the transport service (see ITU-T Rec. X.214 | ISO/IEC 8072).
 *
 * r) The use of selective acknowledgement - This allows the transport entities
 *    to decide whether to use procedures that allow acknowledgement of
 *    DT-TPDUs that are received out-of-sequence (only in class 4).
 *
 * s) The use of request acknowledgement - This allows both transport entities
 *    to negotiate the use or non-use of the request acknowledgement facility
 *    specified in 6.13.4.2 (only in classes 1, 3, 4).
 *
 * t) The use of non-blocking expedite data transfer service - This allows both
 *    transport entities to negotiate the use or non-use of the non-blocking
 *    expedited data transfer service (only in class 4).  This option will only
 *    be valid when the option of "use of expedited data transfer service" is
 *    negotiated.
 *
 * The following information is sent only in the CR-TPDU:
 *
 * u) Version number - This defines the version of the transport protocol
 *    standard used for this connection.
 *
 * v) Reassignment time parameter - This indicates the time for which the
 *    intiiator will persist in following the reassignment after failure
 *    procedure.
 *     
 */
STATIC mblk_t *
tp_pack_cr(struct tp *tp)
{
	register unsigned char *p;
	mblk_t *bp;

	/* 128-32 = 96 octets gives enough room for the entire CR-TPDU */
	if (unlikely((bp = ss7_allocb(q, 128, BPRI_MED)) == NULL))
		return (bp);

	bp->b_datap->db_type = M_DATA;
	p = bp->b_wptr;
	*p++ = 0;		/* fix up later */
	*p++ = _TP_MT_CR | tp->credit;
	*p++ = 0;
	*p++ = 0;
	*p++ = tp->sref >> 8;
	*p++ = tp->sref >> 0;
	*p++ = (tp->opts.tco.tco_prefclass << 4)
	    | (tp->options & (_T_F_CO_EXTFORM | _T_F_CO_FLOWCTRL));
	if (tp->src.len > 0) {
		*p++ = _TP_PT_CGTRANSSEL;	/* calling transport selector */
		*p++ = tp->src.len;
		bcopy(tp->src.buf, p, tp->src.len);
		p += tp->src.len;
	}
	if (tp->dst.len > 0) {
		*p++ = _TP_PT_CDTRANSSEL;	/* called transport selector */
		*p++ = tp->dst.len;
		bcopy(tp->dst.buf, p, tp->dst.len);
		p += tp->dst.len;
	}
	if (tp->opts.tco.tco_ltpdu != T_UNSPEC) {
		int order, t_uscalar_t size;

		size = tp->opts.tco.tco_ltpdu;
		for (order = _T_TPDU_SIZE_8192; order >= _T_TPDU_SIZE_128; order--)
			if ((1 << order) <= size)
				break;
		if (order != _T_TPDU_SIZE_128) {
			*p++ = _TP_PT_TPDU_SIZE;	/* TPDU size */
			*p++ = 1;
			*p++ = order;
		}
		if (!(size >>= 7))
			size = 1;
		*p++ = _TP_PT_PREF_TPDU_SIZE;	/* preferred TPDU size */
		if (size >= 1 << 24) {
			*p++ = 4;
			*p++ = size >> 24;
			*p++ = size >> 16;
			*p++ = size >> 8;
			*p++ = size >> 0;
		} else if (size >= 1 << 16) {
			*p++ = 3;
			*p++ = size >> 16;
			*p++ = size >> 8;
			*p++ = size >> 0;
		} else if (size >= 1 << 8) {
			*p++ = 2;
			*p++ = size >> 8;
			*p++ = size >> 0;
		} else {
			*p++ = 1;
			*p++ = size >> 0;
		}
	}
	if (tp->opts.tco.tco_prefclass != T_CLASS0) {
		*p++ = _TP_PT_VERSION;	/* version number */
		*p++ = 1;
		*p++ = 1;
		if (tp->opts.flags & _T_BIT_TCO_PROTECTION & tp->opts.tco.tco_protection !=
		    T_UNSPEC) {
			*p++ = _TP_PT_PROTECTION;	/* protection */
			*p++ = 1;
			*p++ = tp->opts.tco.tco_protection & ~T_ABSREQ;
		}
		{
			t_uscalar_t addopts = 0;

			*p++ = _TP_PT_ADDOPTIONS;	/* additional options */
			*p++ = 1;
			*p++ = tp->addopts;
		}
		if (tp->network != N_CLNS) {
			int i, altnum = 0;

			for (i = 0; i < 4; i++)
				if ((&tp->opts.tco.tco_altclass1)[i] != T_UNSPEC)
					altnum++;

			if (altnum) {
				*p++ = _TP_PT_ALTCLASS;	/* alternative protocol classes */
				*p++ = altnum;
				for (i = 0; i < 4; i++) {
					if ((&tp->opts.tco.tco_altclass1)[i] != T_UNSPEC)
						*p++ = (&tp->opts.tcp.tco_altclass1)[i];
				}
			}
		}
		if (tp->opts.tco.tco_prefclass == T_CLASS4) {
			*p++ = _TP_PT_ACKTIME;	/* acknowledgement time */
			*p++ = 2;
			*p++ = tp->opts.tco.tco_acktime >> 8;
			*p++ = tp->opts.tco.tco_acktime >> 0;
		}
		if (tp->flags & _TP_MAX_THROUGHPUT) {
			*p++ = _TP_PT_THROUGHPUT;	/* throughput */
			if (!(tp->flags & _TP_AVG_THROUGHPUT)) {
				*p++ = 12;
			} else {
				*p++ = 24;
			}
			*p++ = tp->opts.tco.tco_throughput.maxthrpt.called.targetvalue >> 16;
			*p++ = tp->opts.tco.tco_throughput.maxthrpt.called.targetvalue >> 8;
			*p++ = tp->opts.tco.tco_throughput.maxthrpt.called.targetvalue >> 0;
			*p++ = tp->opts.tco.tco_throughput.maxthrpt.called.minacceptvalue >> 16;
			*p++ = tp->opts.tco.tco_throughput.maxthrpt.called.minacceptvalue >> 8;
			*p++ = tp->opts.tco.tco_throughput.maxthrpt.called.minacceptvalue >> 0;
			*p++ = tp->opts.tco.tco_throughput.maxthrpt.calling.targetvalue >> 16;
			*p++ = tp->opts.tco.tco_throughput.maxthrpt.calling.targetvalue >> 8;
			*p++ = tp->opts.tco.tco_throughput.maxthrpt.calling.targetvalue >> 0;
			*p++ = tp->opts.tco.tco_throughput.maxthrpt.calling.minacceptvalue >> 16;
			*p++ = tp->opts.tco.tco_throughput.maxthrpt.calling.minacceptvalue >> 8;
			*p++ = tp->opts.tco.tco_throughput.maxthrpt.calling.minacceptvalue >> 0;
			if (tp->flags & _TP_AVG_THROUGHPUT) {
				*p++ =
				    tp->opts.tco.tco_throughput.avgthrpt.called.targetvalue >> 16;
				*p++ = tp->opts.tco.tco_throughput.avgthrpt.called.targetvalue >> 8;
				*p++ = tp->opts.tco.tco_throughput.avgthrpt.called.targetvalue >> 0;
				*p++ =
				    tp->opts.tco.tco_throughput.avgthrpt.called.
				    minacceptvalue >> 16;
				*p++ =
				    tp->opts.tco.tco_throughput.avgthrpt.called.minacceptvalue >> 8;
				*p++ =
				    tp->opts.tco.tco_throughput.avgthrpt.called.minacceptvalue >> 0;
				*p++ =
				    tp->opts.tco.tco_throughput.avgthrpt.calling.targetvalue >> 16;
				*p++ =
				    tp->opts.tco.tco_throughput.avgthrpt.calling.targetvalue >> 8;
				*p++ =
				    tp->opts.tco.tco_throughput.avgthrpt.calling.targetvalue >> 0;
				*p++ =
				    tp->opts.tco.tco_throughput.avgthrpt.calling.
				    minacceptvalue >> 16;
				*p++ =
				    tp->opts.tco.tco_throughput.avgthrpt.calling.
				    minacceptvalue >> 8;
				*p++ =
				    tp->opts.tco.tco_throughput.avgthrpt.calling.
				    minacceptvalue >> 0;
			}
		}
		if (tp->flags & _TP_RESIDERRRATE) {
			*p++ = _TP_PT_RESERRORRATE;	/* residual error rate */
			*p++ = 3;
			*p++ = tp->opts.tco.tco_reserrorrate.targetvalue;
			*p++ = tp->opts.tco.tco_reserrorrate.minacceptvalue;
			*p++ = tp->opts.tco.tco_ltpdu;	/* FIXME */
		}
		*p++ = _TP_PT_PRIORITY;	/* priority */
		*p++ = 2;
		*p++ = tp->opts.tco.tco_priority >> 8;
		*p++ = tp->opts.tco.tco_priority >> 0;
		*p++ = _PT_TP_TRANSDEL;	/* transit delay */
		*p++ = 8;
		*p++ = tp->opts.tco.tco_transdel.maxdel.called.targetvalue >> 8;
		*p++ = tp->opts.tco.tco_transdel.maxdel.called.targetvalue >> 0;
		*p++ = tp->opts.tco.tco_transdel.maxdel.called.minacceptvalue >> 8;
		*p++ = tp->opts.tco.tco_transdel.maxdel.called.minacceptvalue >> 0;
		*p++ = tp->opts.tco.tco_transdel.maxdel.calling.targetvalue >> 8;
		*p++ = tp->opts.tco.tco_transdel.maxdel.calling.targetvalue >> 0;
		*p++ = tp->opts.tco.tco_transdel.maxdel.calling.minacceptvalue >> 8;
		*p++ = tp->opts.tco.tco_transdel.maxdel.calling.minacceptvalue >> 0;
		if (tp->opts.tco.tco_prefclass >= T_CLASS3) {
			*p++ = _TP_PT_REASTIME;	/* reassignment time */
			*p++ = 2;
			*p++ = tp->opts.tco.tco_reastime >> 8;
			*p++ = tp->opts.tco.tco_reastime >> 0;
		}
		if (tp->opts.tco.tco_prefclass == T_CLASS4) {
			*p++ = _TP_PT_INACTTIME;	/* inactivity timer */
			*p++ = 4;
			*p++ = tp->intactivty >> 24;
			*p++ = tp->intactivty >> 16;
			*p++ = tp->intactivty >> 8;
			*p++ = tp->intactivty >> 0;
		}
	}
	bp->b_rptr[0] = p - bp->b_rptr - 1;	/* set length indicator */
	bp->b_wptr = p;

#if 0
	/* XXX: caller must add data and do checksum */
	bp->b_cont = dp;
	if (checksum)
		tp_add_checksum(bp);
#endif
	return (bp);
}

/**
 * tp_pack_cc - create a Connection Confirmation (CC) TPDU
 * @tp: sending transport endpoint
 * @q: invoking queue
 *
 * CDT
 * DST-REF: (SRC-REF received in CR)
 * SRC-REF: (set to assigned source reference)
 * CLASS and OPTIONS (selected)
 * Calling Transport Selector;
 * Responding Transport Selector;
 * TPDU size (selected)
 * the preferred maximum TPDU size (selected)
 * protection parameter
 * checksum
 * additional option selection (selected)
 * acknowledgement time
 * inactivity time
 * throughput (selected)
 * residual error rate (selected)
 * priority (selected)
 * transit delay (selected)
 * user data
 */
STATIC mblk_t *
tp_pack_cc(struct tp *tp, queue_t *q)
{
	register unsigned char *p;
	mblk_t *bp;
	int rtn, n = -1;

	/* 128-32 = 96 octets gives enough room for the entire CR-TPDU */
	if ((bp = ss7_allocb(q, 96, BPRI_MED)) == NULL)
		goto enobufs;
	p = bp->b_wptr;
	*p++ = 0;		/* fix up later */
	*p++ = _TP_MT_CC | tp->credit;
	*p++ = tp->dref >> 8;
	*p++ = tp->dref >> 0;
	*p++ = tp->sref >> 8;
	*p++ = tp->sref >> 0;
	*p++ = (tp->tp_options.tco.tco_prefclass << 4)
	    | (tp->options & (_T_F_CO_EXTFORM | _T_F_CO_FLOWCTRL));
	if (cgts_len > 0) {
		*p++ = _TP_PT_CGTRANSSEL;	/* calling transport selector */
		*p++ = cgts_len;
		bcopy(cgts_ptr, p, cgts_len);
		p += cgts_len;
	}
	if (cdts_len > 0) {
		*p++ = _TP_PT_CDTRANSSEL;	/* called transport selector */
		*p++ = cdts_len;
		bcopy(cdts_ptr, p, cdts_len);
		p += cdts_len;
	}
	if (tp->tp_options.tco.tco_ltpdu != T_UNSPEC) {
		int order, t_uscalar_t size;

		size = tp->tp_options.tco.tco_ltpdu;
		for (order = _T_TPDU_SIZE_8192; order >= _T_TPDU_SIZE_128; order--)
			if ((1 << order) <= size)
				break;
		if (order != _T_TPDU_SIZE_128) {
			*p++ = _TP_PT_TPDU_SIZE;	/* TPDU size */
			*p++ = 1;
			*p++ = order;
		}
		if (!(size >>= 7))
			size = 1;
		*p++ = _TP_PT_PREF_TPDU_SIZE;	/* preferred TPDU size */
		if (size >= 1 << 24) {
			*p++ = 4;
			*p++ = size >> 24;
			*p++ = size >> 16;
			*p++ = size >> 8;
			*p++ = size >> 0;
		} else if (size >= 1 << 16) {
			*p++ = 3;
			*p++ = size >> 16;
			*p++ = size >> 8;
			*p++ = size >> 0;
		} else if (size >= 1 << 8) {
			*p++ = 2;
			*p++ = size >> 8;
			*p++ = size >> 0;
		} else {
			*p++ = 1;
			*p++ = size >> 0;
		}
	}
	if (tp->tp_options.tco.tco_prefclass != T_CLASS0) {
#if 0
		*p++ = _TP_PT_VERSION;	/* version number */
		*p++ = 1;
		*p++ = 1;
#endif
		if (tp->tp_options.tco.tco_protection) {
			*p++ = _TP_PT_PROTECTION;	/* protection */
			*p++ = 1;
			*p++ = tp->tp_options.tco.tco_protection;
		}
		*p++ = _TP_PT_ADDOPTIONS;	/* additional options */
		*p++ = 1;
		*p++ = tp->addopts;
#if 0
		if (tp->network != N_CLNS) {
			int i, altnum = 0;

			for (i = 0; i < 4; i++)
				if ((&tp->tp_options.tco.tco_altclass1)[i] != T_UNSPEC)
					altnum++;

			if (altnum) {
				*p++ = _TP_PT_ALTCLASS;	/* alternative protocol classes */
				*p++ = altnum;
				for (i = 0; i < 4; i++) {
					if ((&tp->tp_options.tco.tco_altclass1)[i] != T_UNSPEC)
						*p++ = (&tp->tp_options.tcp.tco_altclass1)[i];
				}
			}
		}
#endif
		if (tp->tp_options.tco.tco_prefclass == T_CLASS4) {
			*p++ = _TP_PT_ACKTIME;	/* acknowledgement time */
			*p++ = 2;
			*p++ = tp->tp_options.tco.tco_acktime >> 8;
			*p++ = tp->tp_options.tco.tco_acktime >> 0;
		}
		if (tp->flags & _TP_MAX_THROUGHPUT) {
			*p++ = _TP_PT_THROUGHPUT;	/* throughput */
			if (!(tp->flags & _TP_AVG_THROUGHPUT)) {
				*p++ = 12;
			} else {
				*p++ = 24;
			}
			*p++ = tp->tp_options.tco.tco_throughput.maxthrpt.called.targetvalue >> 16;
			*p++ = tp->tp_options.tco.tco_throughput.maxthrpt.called.targetvalue >> 8;
			*p++ = tp->tp_options.tco.tco_throughput.maxthrpt.called.targetvalue >> 0;
			*p++ =
			    tp->tp_options.tco.tco_throughput.maxthrpt.called.minacceptvalue >> 16;
			*p++ =
			    tp->tp_options.tco.tco_throughput.maxthrpt.called.minacceptvalue >> 8;
			*p++ =
			    tp->tp_options.tco.tco_throughput.maxthrpt.called.minacceptvalue >> 0;
			*p++ = tp->tp_options.tco.tco_throughput.maxthrpt.calling.targetvalue >> 16;
			*p++ = tp->tp_options.tco.tco_throughput.maxthrpt.calling.targetvalue >> 8;
			*p++ = tp->tp_options.tco.tco_throughput.maxthrpt.calling.targetvalue >> 0;
			*p++ =
			    tp->tp_options.tco.tco_throughput.maxthrpt.calling.minacceptvalue >> 16;
			*p++ =
			    tp->tp_options.tco.tco_throughput.maxthrpt.calling.minacceptvalue >> 8;
			*p++ =
			    tp->tp_options.tco.tco_throughput.maxthrpt.calling.minacceptvalue >> 0;
			if (tp->flags & _TP_AVG_THROUGHPUT) {
				*p++ =
				    tp->tp_options.tco.tco_throughput.avgthrpt.called.
				    targetvalue >> 16;
				*p++ =
				    tp->tp_options.tco.tco_throughput.avgthrpt.called.
				    targetvalue >> 8;
				*p++ =
				    tp->tp_options.tco.tco_throughput.avgthrpt.called.
				    targetvalue >> 0;
				*p++ =
				    tp->tp_options.tco.tco_throughput.avgthrpt.called.
				    minacceptvalue >> 16;
				*p++ =
				    tp->tp_options.tco.tco_throughput.avgthrpt.called.
				    minacceptvalue >> 8;
				*p++ =
				    tp->tp_options.tco.tco_throughput.avgthrpt.called.
				    minacceptvalue >> 0;
				*p++ =
				    tp->tp_options.tco.tco_throughput.avgthrpt.calling.
				    targetvalue >> 16;
				*p++ =
				    tp->tp_options.tco.tco_throughput.avgthrpt.calling.
				    targetvalue >> 8;
				*p++ =
				    tp->tp_options.tco.tco_throughput.avgthrpt.calling.
				    targetvalue >> 0;
				*p++ =
				    tp->tp_options.tco.tco_throughput.avgthrpt.calling.
				    minacceptvalue >> 16;
				*p++ =
				    tp->tp_options.tco.tco_throughput.avgthrpt.calling.
				    minacceptvalue >> 8;
				*p++ =
				    tp->tp_options.tco.tco_throughput.avgthrpt.calling.
				    minacceptvalue >> 0;
			}
		}
		if (tp->flags & _TP_RESIDERRRATE) {
			*p++ = _TP_PT_RESERRORRATE;	/* residual error rate */
			*p++ = 3;
			*p++ = tp->tp_options.tco.tco_reserrorrate.targvalue;
			*p++ = tp->tp_options.tco.tco_reserrorrate.minacceptvalue;
			*p++ = tp->tp_options.tco.tco_ltpdu;	/* FIXME */
		}
		*p++ = _TP_PT_PRIORITY;	/* priority */
		*p++ = 2;
		*p += = tp->tp_options.tco.tco_priority >> 8;
		*p += = tp->tp_options.tco.tco_priority >> 0;
		*p++ = _PT_TP_TRANSDEL;	/* transit delay */
		*p++ = 8;
		*p++ = tp->tp_options.tco.tco_transdel.maxdel.called.targetvalue >> 8;
		*p++ = tp->tp_options.tco.tco_transdel.maxdel.called.targetvalue >> 0;
		*p++ = tp->tp_options.tco.tco_transdel.maxdel.called.minacceptvalue >> 8;
		*p++ = tp->tp_options.tco.tco_transdel.maxdel.called.minacceptvalue >> 0;
		*p++ = tp->tp_options.tco.tco_transdel.maxdel.calling.targetvalue >> 8;
		*p++ = tp->tp_options.tco.tco_transdel.maxdel.calling.targetvalue >> 0;
		*p++ = tp->tp_options.tco.tco_transdel.maxdel.calling.minacceptvalue >> 8;
		*p++ = tp->tp_options.tco.tco_transdel.maxdel.calling.minacceptvalue >> 0;
#if 0
		if (tp->tp_options.tco.tco_prefclass >= T_CLASS3) {
			*p++ = _TP_PT_REASTIME;	/* reassignment time */
			*p++ = 2;
			*p++ = tp->tp_options.tco.tco_reastime >> 8;
			*p++ = tp->tp_options.tco.tco_reastime >> 0;
		}
#endif
		if (tp->tp_options.tco.tco_prefclass == T_CLASS4) {
			*p = _TP_PT_INACTTIME;	/* inactivity timer */
			*p++ = 4;
			*p++ = tp->intactivty >> 24;
			*p++ = tp->intactivty >> 16;
			*p++ = tp->intactivty >> 8;
			*p++ = tp->intactivty >> 0;
		}
	}
	bp->b_rptr[0] = p - bp->b_rptr - 1;	/* set length indicator */
	bp->b_wptr = p;

#if 0
	/* FIXME: let caller add data and do checksum */
	bp->b_cont = dp;
	if (checksum)
		tp_add_checksum(bp);
#endif
      enobufs:
	return (bp);
}

/**
 * tp_pack_dr - Send a Disconnect Request (DR) Message
 * @tp: sending transport endpoint
 * @q: invoking queue
 */
STATIC mblk_t *
tp_pack_dr(struct tp *tp, queue_t *q, t_uscalar_t reason, struct netbuf *diag)
{
	register unsigned char *p;
	mblk_t *bp;
	int rtn;

	if ((bp = ss7_allocb(q, 13 + diag->len, BPRI_MED))) {
		p = bp->b_wptr;
		*p++ = 0;	/* fix up later */
		*p++ = _TP_MT_DR;	/* disconnect request */
		*p++ = tp->dref >> 8;	/* destination reference */
		*p++ = tp->dref >> 0;
		*p++ = tp->sref >> 8;	/* source reference */
		*p++ = tp->sref >> 0;
		*p++ = reason;
		if (diag->len > 0 && diag->buf != NULL) {
			*p++ = _TP_PT_DIAGNOSTIC;	/* diagnostic */
			*p++ = diag->len;
			bcopy(diag->buf, p, diag->len);
			p += diag->len;
		}
		*bp->b_rptr = p - bp->b_rptr - 1;	/* set length indicator */
		bp->b_wptr = p;

#if 0
		/* FIXME: let caller add data and do checksum */
		bp->b_cont = dp;
		if (checksum)
			tp_add_checksum(bp);
#endif
	}
	return (bp);

}

/**
 * tp_pack_dc - Send a Disconnect Confirmation (DC) Message
 * @tp: sending transport endpoint
 * @q: invoking queue
 */
STATIC mblk_t *
tp_pack_dc(struct tp *tp, queue_t *q)
{
	register unsigned char *p;
	mblk_t *bp;
	int rtn;

	if ((bp = ss7_allocb(q, 10, BPRI_MED))) {
		p = bp->b_wptr;
		*p++ = 0;	/* fix up later */
		*p++ = _TP_MT_DC;	/* disconnect confirm */
		*p++ = tp->dref >> 8;	/* destination reference */
		*p++ = tp->dref >> 0;
		*p++ = tp->sref >> 8;	/* source reference */
		*p++ = tp->sref >> 0;
		bp->b_rptr[0] = p - bp->b_rptr - 1;	/* set length indicator */
		bp->b_wptr = p;

#if 0
		/* FIXME: let caller add data and do checksum */
		bp->b_cont = dp;
		if (checksum)
			tp_add_checksum(bp);
#endif
	}
	return (bp);
}

/**
 * tp_pack_dt - Send a Data (DT) Message
 * @tp: sending transport endpoint
 * @q: invoking queue
 * @roa: receipt acknowlegement option
 * @eot: end of transmission
 * @nr: sequence number
 * @ednr: expedited data sequence number
 */
STATIC mblk_t *
tp_pack_dt(queue_t *q, uint8_t roa, uint8_t eot, uint32_t nr, int ednr, mblk_t *dp)
{
	register unsigned char *p;
	struct tp *tp = TP_PRIV(q);
	mblk_t *bp;
	int rtn;

	if ((bp = ss7_allocb(q, FIXME, BPRI_MED))) {
		p = bp->b_wptr;
		*p++ = 0;	/* fix up later */
		*p++ = _TP_MT_DT | roa;	/* data */
		switch (tp->tp_options.tco.tco_prefclass) {
		case T_CLASS0:
		case T_CLASS1:
			*p++ = (eot ? 0x80 : 0x00) | (nr & 0x7f);
			break;
		case T_CLASS2:
		case T_CLASS3:
		case T_CLASS4:
			*p++ = tp->dref >> 8;
			*p++ = tp->dref >> 0;
			if (!(tp->options & _T_F_CO_EXTFORM)) {
				*p++ = (eot ? 0x80 : 0x00) | (nr & 0x7f);
			} else {
				*p++ = (eot ? 0x80 : 0x00) | ((nr >> 24) & 0x7f);
				*p++ = nr >> 16;
				*p++ = nr >> 8;
				*p++ = nr >> 0;
			}
			if ((tp->addopts & _T_F_AO_NBLKEXPDATA) && ednr != -1) {
				*p++ = _TP_PT_ED_TPDU_NR;	/* ED-TPDU-NR */
				if (!(tp->options & _T_F_CO_EXTFORM)) {
					*p++ = 2;
					*p++ = ednr >> 8;
					*p++ = ednr >> 0;
				} else {
					*p++ = 4;
					*p++ = ednr >> 24;
					*p++ = ednr >> 16;
					*p++ = ednr >> 8;
					*p++ = ednr >> 0;
				}
			}
			break;
		}
		bp->b_rptr[0] = p - bp->b_rptr - 1;	/* set length indicator */
		bp->b_wptr = p;

#if 0
		/* FIXME: let caller add data and do checksum */
		bp->b_cont = dp;
		if (checksum)
			tp_add_checksum(bp);
#endif
	}
	return (bp);
}

/**
 * tp_pack_ed - Send an Expedited Data (ED) Message
 * @tp: sending transport endpoint
 * @q: invoking queue
 * @nr: sequence number
 */
STATIC mblk_t *
tp_pack_ed(queue_t *q, uint32_t nr, mblk_t *dp)
{
	register unsigned char *p;
	struct tp *tp = TP_PRIV(q);
	mblk_t *bp;
	int rtn;

	if ((bp = ss7_allocb(q, FIXME, BPRI_MED)) == NULL)
		goto enobufs;

	// bp->b_datap->db_type = M_DATA; /* redundant */
	p = bp->b_wptr;
	*p++ = 0;		/* fix up later */
	*p++ = _TP_MD_ED;	/* expedited data */
	*p++ = tp->dref >> 8;
	*p++ = tp->dref >> 0;
	assure(tp->tp_options.tco.tco_prefclass != T_CLASS0);
	if (!(tp->options & _T_F_CO_EXTFORM)) {
		*p++ = 0x80 | (nr & 0x7f);
	} else {
		*p++ = 0x80 | ((nr >> 24) & 0x7f);
		*p++ = nr >> 16;
		*p++ = nr >> 8;
		*p++ = nr >> 0;
	}
	bp->b_rptr[0] = p - bp->b_rptr - 1;	/* set length indicator */
	bp->b_wptr = p;

	/* FIXME: let caller add data and do checksum */
	bp->b_cont = dp;
	if (checksum)
		tp_add_checksum(bp);
      enobufs:
	return (bp);
}

/**
 * tp_pack_ak - Send an Acknowledgement (AK) Message
 * @tp: sending transport endpoint
 * @q: invoking queue
 * @nr: sequence number
 */
STATIC mblk_t *
tp_pack_ak(queue_t *q, unsigned short options, unsigned short dref, uint16_t credit, uint32_t nr,
	   int ssn)
{
	register unsigned char *p;
	struct tp *tp = TP_PRIV(q);
	mblk_t *bp;
	int rtn;

	if ((bp = ss7_allocb(q, FIXME, BPRI_MED)) == NULL)
		goto enobufs;

	// bp->b_datap->db_type = M_DATA; /* redundant */
	p = bp->b_wptr;
	*p++ = 0;		/* fix up later */
	if (!(options & (_T_F_CO_EXTFORM << 8))) {
		*p++ = _TP_MT_AK | (credit & 0x0f);	/* acknowledge */
	} else {
		*p++ = _TP_MT_AK;	/* acknowledge */
	}
	*p++ = dref >> 8;
	*p++ = dref >> 0;
	if (!(options & (_T_F_CO_EXTFORM << 8))) {
		*p++ = nr & 0x7f;
	} else {
		*p++ = (nr >> 24) & 0x7f;
		*p++ = nr >> 16;
		*p++ = nr >> 8;
		*p++ = nr >> 0;
		*p++ = credit >> 8;
		*p++ = credit >> 0;
	}
	if (tp->tp_options.tco.tco_prefclass == T_CLASS4) {
		if (ssn != 0) {
			*p++ = _TP_PT_SUBSEQUENCE;	/* subsequence number */
			*p++ = 2;
			*p++ = ssn >> 8;
			*p++ = ssn >> 0;
		}
		if (options & (_T_F_CO_FLOWCTRL << 8)) {
			*p++ = _TP_PT_FLOWCTLCF;	/* flow control confirmation */
			*p++ = 8;
			if (!(options & (_T_F_CO_EXTFORM << 8))) {
				*p++ = 0;
				*p++ = 0;
				*p++ = 0;
				*p++ = rnr & 0x7f;
			} else {
				*p++ = (rnr >> 24) & 0x7f;
				*p++ = rnr >> 16;
				*p++ = rnr >> 8;
				*p++ = rnr >> 0;
			}
			*p++ = rssn >> 8;
			*p++ = rssn >> 0;
			if (!(options & (_T_F_CO_EXTFORM << 8))) {
				*p++ = 0;
				*p++ = 0;
				*p++ = 0;
				*p++ = credit & 0x7f;
			} else {
				*p++ = (credit >> 24) & 0x7f;
				*p++ = credit >> 16;
				*p++ = credit >> 8;
				*p++ = credit >> 0;
			}
		}
		if (options & _T_F_AO_SELECTACK) {
			*p++ = _TP_PT_SELECTACK;	/* selective acknowledgement */
			if (!(options & (_T_F_CO_EXTFORM << 8))) {
				*p++ = tp->sackblks << 1;
				while (FIXME) {
					*p++ = blk->beg & 0x7f;
					*p++ = blk->end & 0x7f;
				}
			} else {
				*p++ = tp->sackblks << 3;
				while (FIXME) {
					*p++ = (blk->beg >> 24) & 0x7f;
					*p++ = blk->beg >> 16;
					*p++ = blk->beg >> 8;
					*p++ = blk->beg >> 0;
					*p++ = (blk->end >> 24) & 0x7f;
					*p++ = blk->end >> 16;
					*p++ = blk->end >> 8;
					*p++ = blk->end >> 0;
				}
			}
		}
	}
	bp->b_rptr[0] = p - bp->b_rptr - 1;	/* set length indicator */
	bp->b_wptr = p;

	/* FIXME: let caller add data and do checksum */
	bp->b_cont = dp;
	if (checksum)
		tp_add_checksum(bp);
      enobufs:
	return (bp);
}

/**
 * tp_pack_ea - Send an Expedited Data Acknowledgement (EA) Message
 * @tp: sending transport endpoint
 * @q: invoking queue
 * @nr: sequence number
 */
STATIC mblk_t *
tp_pack_ea(queue_t *q, unsigned short options, unsigned short dref, uint32_t nr, int ssn)
{
	register unsigned char *p;
	mblk_t *bp;
	int rtn;

	if ((bp = ss7_allocb(q, FIXME, BPRI_MED))) {
		p = bp->b_wptr;
		*p++ = 0;	/* fix up later */
		*p++ = _TP_MT_EA;	/* expedited data acknowledge */
		*p++ = dref >> 8;
		*p++ = dref >> 0;
		if (!(options & (_T_F_CO_EXTFORM << 8))) {
			*p++ = nr & 0x7f;
		} else {
			*p++ = (nr >> 24) & 0x7f;
			*p++ = nr >> 16;
			*p++ = nr >> 8;
			*p++ = nr >> 0;
		}
		bp->b_rptr[0] = p - bp->b_rptr - 1;	/* set length indicator */
		bp->b_wptr = p;

		/* FIXME: let caller add data and do checksum */
		bp->b_cont = NULL;
		if (checksum)
			tp_add_checksum(bp);
	}
	return (bp);
}

/**
 * tp_pack_rj - Send a Reject (RJ) Message
 * @tp: sending transport endpoint
 * @q: invoking queue
 * @nr: sequence number
 */
STATIC mblk_t *
tp_pack_rj(queue_t *q, uint32_t nr, int ssn)
{
	register unsigned char *p;
	struct tp *tp = TP_PRIV(q);
	mblk_t *bp;
	int rtn;

	if ((bp = ss7_allocb(q, FIXME, BPRI_MED))) {
		p = bp->b_wptr;
		*p++ = 0;	/* fix up later */
		if (tp_normal(tp)) {
			*p++ = _TP_MT_RJ | (tp->credit & 0x0f);	/* reject */
		} else {
			*p++ = _TP_MT_RJ;	/* reject */
		}
		*p++ = tp->dref >> 8;
		*p++ = tp->dref >> 0;
		if (tp_normal(tp)) {
			*p++ = nr & 0x7f;
		} else {
			*p++ = (nr >> 24) & 0x7f;
			*p++ = nr >> 16;
			*p++ = nr >> 8;
			*p++ = nr >> 0;
			*p++ = tp->credit >> 8;
			*p++ = tp->credit >> 0;
		}
		bp->b_rptr[0] = p - bp->b_rptr - 1;	/* set length indicator */
		bp->b_wptr = p;

		/* FIXME: let caller add data and do checksum */
		bp->b_cont = dp;
		if (checksum)
			tp_add_checksum(bp);
	}
	return (bp);
}

/**
 * tp_pack_er - Create an Error (ER) TPDU
 * @tp: sending transport endpoint
 * @q: invoking queue
 * @cause: reject cause
 */
STATIC mblk_t *
tp_pack_er(queue_t *q, uint32_t cause, struct netbuf *tpdu)
{
	register unsigned char *p;
	struct tp *tp = TP_PRIV(q);
	mblk_t *bp;
	int rtn, n = -1;

	if ((bp = ss7_allocb(q, FIXME, BPRI_MED))) {
		p = bp->b_wptr;
		*p++ = 0;	/* fix up later */
		*p++ = _TP_MT_ER;	/* error */
		*p++ = tp->dref >> 8;
		*p++ = tp->dref >> 0;
		*p++ = cause;
		if (tpud->len > 0 && tpdu->buf != NULL) {
			*p++ = _TP_PT_INVALID_TPDU;	/* invalid TPDU */
			*p++ = tpud->len;
			bcopy(tpdu->buf, p, tpud->len);
			p += tpud->len;
		}
		*bp->b_rptr = p - bp->b_rptr - 1;	/* set length indicator */
		bp->b_wptr = p;

		/* FIXME: let caller add data and do checksum */
		bp->b_cont = dp;
		if (checksum)
			tp_add_checksum(bp);
	}
	return (bp);
}

/**
 * tp_pack_ud - Create a Unit Data (UD) TPDU
 * @tp: sending transport endpoint
 * @q: invoking queue
 */
STATIC INLINE fastcall __hot_put mblk_t *
tp_pack_ud(struct tp *tp, queue_t *q)
{
	register unsigned char *p;
	mblk_t *bp;
	int rtn, n = -1;

	if ((bp = ss7_allocb(q, FIXME, BPRI_MED))) {
		p = bp->b_wptr;
		*p++ = 0;	/* length indicator - fix up later */
		*p++ = _TP_MT_UD;	/* unitdata */
		if (tp->dst.len > 0) {
			*p++ = _TP_PT_CDTRANSSEL;
			*p++ = tp->dst.len;
			bcopy(tp->dst.buf, p, tp->dst.len);
			p += tp->dst.len;
		}
		if (tp->src.len > 0) {
			*p++ = _TP_PT_CGTRANSSEL;
			*p++ = tp->src.len;
			bcopy(tp->src.buf, p, tp->src.len);
			p += tp->src.len;
		}
		*bp->b_rptr = p - bp->b_rptr - 1;	/* set length indicator */
		bp->b_wptr = p;
	}
	return (bp);
}

/*
 *  TS-Provider Primitives sent upstream.
 *  -------------------------------------
 */

/**
 * te_conn_ind - generate a TE_CONN_IND event
 * @q: invoking queue
 * @dp: data block containing user data
 */
STATIC int
te_conn_ind(queue_t *q, mblk_t *dp)
{
}

/**
 * te_conn_con - generate a TE_CONN_CON event
 * @q: invoking queue
 * @dp: data block containing user data
 */
STATIC int
te_conn_con(queue_t *q, mblk_t *dp)
{
}

/**
 * te_discon_ind - generate a TE_DISCON_IND event
 * @q: output queue
 * @reason: disconnect reason
 * @sequence: disconnect sequence number
 * @dp: data block containing user data
 *
 * Flow Control: Note that if the allocation fails, the output queue will be envabled when a buffer
 * is available.  The upper service procedure, however, also wakes up all lower feeding queues, so
 * this is ok.
 */
STATIC int
te_discon_ind(queue_t *q, t_uscalar_t reason, t_uscalar_t sequence, mblk_t *dp)
{
	mblk_t *mp;
	struct T_discon_ind *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		mp->b_band = 2;
		p = (struct T_discon_ind *) mp->b_rptr;
		p->PRIM_type = T_DISCON_IND;
		p->DISCON_reason = reason;
		p->SEQ_number = sequence;
		mp->b_wptr += sizeof(*p);
		mp->b_cont = dp;
		put(q, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * te_data_ind - generate a TE_DATA_IND event
 * @q: invoking queue
 * @dp: data block containing user data
 */
STATIC INLINE fastcall __hot_get int
te_data_ind(queue_t *q, mblk_t *dp)
{
}

/**
 * te_exdata_ind - generate a TE_EXDATA_IND event
 * @q: invoking queue
 * @dp: data block containing user data
 */
STATIC INLINE fastcall __hot_get int
te_exdata_ind(queue_t *q, mblk_t *dp)
{
}

/**
 * te_optdata_ind - generate a TE_OPTDATA_IND event
 * @q: invoking queue
 * @dp: data block containing user data
 */
STATIC INLINE fastcall __hot_get int
te_optdata_ind(queue_t *q, mblk_t *dp)
{
}

/**
 * te_info_ack - generate a TE_INFO_ACK event
 * @q: invoking queue
 * @dp: data block containing user data
 */
STATIC int
te_info_ack(queue_t *q)
{
}

/**
 * te_bind_ack - generate a TE_BIND_ACK event
 * @q: invoking queue
 * @dp: data block containing user data
 */
STATIC int
te_bind_ack(queue_t *q)
{
}

/**
 * te_error_ack - generate a TE_ERROR_ACK event
 * @q: invoking queue
 * @dp: data block containing user data
 */
STATIC int
te_error_ack(queue_t *q)
{
}

/**
 * te_ok_ack - generate a TE_OK_ACK event
 * @q: invoking queue
 * @dp: data block containing user data
 */
STATIC int
te_ok_ack(queue_t *q, mblk_t *dp)
{
	struct tp *tp = TP_PRIV(q);

	switch (tpi_get_state(tp)) {
	case TS_WACK_CREQ:
	}
}

/**
 * te_unitdata_ind - generate a TE_UNITDATA_IND event
 * @q: invoking queue
 * @dp: data block containing user data
 */
STATIC INLINE fastcall __hot_get int
te_unitdata_ind(queue_t *q, mblk_t *dp)
{
}

/**
 * te_uderror_ind - generate a TE_UDERROR_IND event
 * @q: invoking queue
 * @dp: data block containing user data
 */
STATIC INLINE fastcall __hot_get int
te_uderror_ind(queue_t *q, mblk_t *dp)
{
}

/**
 * te_ordrel_ind - generate a TE_ORDREL_IND event
 * @q: invoking queue
 * @dp: data block containing user data
 */
STATIC int
te_ordrel_ind(queue_t *q, mblk_t *dp)
{
}

/*
 *  TS-User Primitives received from upstream.
 *  ------------------------------------------
 */

/**
 * te_unitdata_req - process an TE_UNITDATA_REQ event
 * @q: the upper multiplex queue on which the message arrived
 * @mp: the T_UNITDATA_REQ message
 */
STATIC INLINE fastcall __hot_put int
te_unitdata_req(queue_t *q, mblk_t *mp)
{
	struct T_unitdata_req *p;
	struct tp *tp = TP_PRIV(q);

	if (mp->b_wptr < mp->b_wptr + sizeof(*p))
		goto eproto;

	p = (struct T_unitdata_req *) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + p->DEST_offset + p->DEST_length)
		goto badaddr;
	if (mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length)
		goto badopt;
	if (mp->b_cont == NULL)
		goto badddata;

	if (mp->b_cont) {
		t_uscalar_t mlen;

		mlen = msgsize(mp->b_cont);
		if (mlen == 0 && !(tp->info.PROVIDER_flag & T_SNDZERO))
			goto baddata;
		if (mlen > tp->info.TSDU_size && tp->info.TSDU_size != T_INFINITE)
			goto baddata;
	}
	if (p->DEST_length) {
		/* Address consists of a (possibly null) TSAP Selector and an NSAP Address. */
		/* TSAP Selector is a possibly 20 byte string. */
		/* NSAP Address is an IDC + DSP.  There are IDC + DSP combinations defined for IP
		   based networks. */
	}
	if (p->OPT_length) {
	}

      baddata:
	err = TBADDATA;
	goto error;
      badopt:
	err = TBADOPT;
	goto error;
      badddr:
	err = TBADADDR;
	goto error;
      eproto:
	err = -EPROTO;
	goto error;
      error:
	return te_fatal_error(q, T_DATA_REQ, err);
}

/**
 * te_data_req - process an TE_DATA_REQ event
 * @q: the upper multiplex queue on which the message arrived
 * @mp: the T_DATA_REQ or M_DATA message
 */
STATIC INLINE fastcall __hot_put int
te_data_req(queue_t *q, mblk_t *mp)
{
}

/**
 * te_conn_req - process an TE_CONN_REQ event
 * @q: the upper multiplex queue on which the message arrived
 * @mp: the T_CONN_REQ message
 */
STATIC int
te_conn_req(queue_t *q, mblk_t *mp)
{
	struct tp *tp = TP_PRIV(q);
	struct T_conn_req *p;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	p = (struct T_conn_req *) mp->b_rptr;
	if (tpi_get_state(tp) != TS_IDLE)
		goto toutstate;
	if (tp->service != T_COTS)
		goto tnotsupport;
	if (mp->b_cont) {
		size_t size = msgsize(mp->b_cont);

		if (size > 0) {
			/* User data not available in Class 0 */
			if (tp->tp_options.tco.tco_prefclass == T_CLASS0)
				goto tbaddata;
			/* Up to 32 octets in other classes. */
			if (size > 32)
				goto tbaddata;
			/* NOTE 3 - If class 0 is a valid response according to Table 3, inclusion
			   of user data in the CR-TPDU may cause the responding entity to refuse
			   the connection (for example, if it only supports class 0). */
		}
	}
	if (p->DEST_length) {
		if (mp->b_wptr < mp->b_rptr + p->DEST_offset + p->DEST_length)
			goto tbadaddr;
		/* TODO: validate the address */
		/* TODO: place the destination address in the private structure */
	} else {
		/* Need an address to use connectionless network. */
		if (tp->network == N_CLNS)
			goto tnoaddr;
		/* If there is more than one network connection supporting this transport, then we
		   need a transport address. */
		if (tp->netcons > 1)
			goto tnoaddr;
		/* If the underlying network provider is already connected, then the transport
		   address is not required. */
		if (tpi_get_n_state(tp) != NS_DATA_XFER)
			goto tnoaddr;
		/* Otherwise, we don't need a transport address. */
		/* TODO: clear the transport address from the private structure */
	}
	if (p->OPT_length) {
		if (mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length)
			goto tbadopt;
		/* TODO: validate the options */
		/* TODO: parse the options into the private structure */
	} else {
		/* Use default options. */
		/* TODO: set default options in the private structure */
	}
	tpi_set_state(tp, TS_WACK_CREQ);
	return te_ok_ack(q, mp->b_cont);
      tbadopt:
	err = TBADOPT;
	goto error;
      tnoaddr:
	err = TNOADDR;
	goto error;
      tbadaddr:
	err = TBADADDR;
	goto error;
      tbaddata:
	err = TBADDATA;
	goto error;
      tnotsupport:
	err = TNOTSUPPORT;
	goto error;
      toutstate:
	err = TOUTSTATE;
	goto error;
      error:
	return te_error_ack(q, T_CONN_REQ, err);
}

/**
 * te_conn_res - process an TE_CONN_RES event
 * @q: the upper multiplex queue on which the message arrived
 * @mp: the T_CONN_RES message
 */
STATIC int
te_conn_res(queue_t *q, mblk_t *mp)
{
}

/**
 * te_discon_req - process an TE_DISCON_REQ event
 * @q: the upper multiplex queue on which the message arrived
 * @mp: the T_DISCON_REQ message
 */
STATIC int
te_discon_req(queue_t *q, mblk_t *mp)
{
}

/**
 * te_ordrel_req - process an TE_ORDREL_REQ event
 * @q: the upper multiplex queue on which the message arrived
 * @mp: the T_ORDREL_REQ message
 */
STATIC int
te_ordrel_req(queue_t *q, mblk_t *mp)
{
}

/**
 * te_exdata_req - process an TE_EXDATA_REQ event
 * @q: the upper multiplex queue on which the message arrived
 * @mp: the T_EXDATA_REQ message
 */
STATIC INLINE fastcall __hot_put int
te_exdata_req(queue_t *q, mblk_t *mp)
{
}

/**
 * te_optdata_req - process an TE_OPTDATA_REQ event
 * @q: the upper multiplex queue on which the message arrived
 * @mp: the T_OPTDATA_REQ message
 */
STATIC INLINE fastcall __hot_put int
te_optdata_req(queue_t *q, mblk_t *mp)
{
}

/**
 * te_info_req - process an TE_INFO_REQ event
 * @q: the upper multiplex queue on which the message arrived
 * @mp: the T_INFO_REQ message
 */
STATIC int
te_info_req(queue_t *q, mblk_t *mp)
{
}

/**
 * te_bind_req - process an TE_BIND_REQ event
 * @q: the upper multiplex queue on which the message arrived
 * @mp: the T_BIND_REQ message
 */
STATIC int
te_bind_req(queue_t *q, mblk_t *mp)
{
}

/**
 * te_unbind_req - process an TE_UNBIND_REQ event
 * @q: the upper multiplex queue on which the message arrived
 * @mp: the T_UNBIND_REQ message
 */
STATIC int
te_unbind_req(queue_t *q, mblk_t *mp)
{
}

/**
 * te_optmgmt_req - process an TE_OPTMGMT_REQ event
 * @q: the upper multiplex queue on which the message arrived
 * @mp: the T_OPTMGMT_REQ message
 */
STATIC INLINE fastcall __hot_put int
te_optmgmt_req(queue_t *q, mblk_t *mp)
{
}

STATIC INLINE fastcall __hot_put int
tp_w_data(queue_t *q, mblk_t *mp)
{
	return te_data_req(q, mp);
}
STATIC INLINE fastcall __hot_put int
tp_w_proto(queue_t *q, mblk_t *mp)
{
	int rtn = -EPROTO;
	t_scalar_t prim = 0;
	struct tp *tp = TP_PRIV(q);
	t_scalar_t oldstate = tpi_get_state(tp);

	if (mp->b_wptr >= mp->b_rptr + sizeof(prim)) {
		switch ((prim = *((t_scalar_t *) mp->b_rptr))) {
		case T_UNITDATA_REQ:
			rtn = te_unitdata_req(q, mp);
			break;
		case T_DATA_REQ:
			rtn = te_data_req(q, mp);
			break;
		case T_CONN_REQ:
			rtn = te_conn_req(q, mp);
			break;
		case T_CONN_RES:
			rtn = te_conn_res(q, mp);
			break;
		case T_DISCON_REQ:
			rtn = te_discon_req(q, mp);
			break;
		case T_ORDREL_REQ:
			rtn = te_ordrel_req(q, mp);
			break;
		case T_EXDATA_REQ:
			rtn = te_exdata_req(q, mp);
			break;
		case T_OPTDATA_REQ:
			rtn = te_optdata_req(q, mp);
			break;
		case T_INFO_REQ:
			rtn = te_info_req(q, mp);
			break;
		case T_BIND_REQ:
			rtn = te_bind_req(q, mp);
			break;
		case T_UNBIND_REQ:
			rtn = te_unbind_req(q, mp);
			break;
		case T_OPTMGMT_REQ:
			rtn = te_optmgmt_req(q, mp);
			break;
		case T_CONN_IND:
		case T_CONN_CON:
		case T_DISCON_IND:
		case T_DATA_IND:
		case T_EXDATA_IND:
		case T_OPTDATA_IND:
		case T_INFO_ACK:
		case T_BIND_ACK:
		case T_ERROR_ACK:
		case T_OK_ACK:
		case T_UNITDATA_IND:
		case T_UDERROR_IND:
		case T_ORDREL_IND:
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
		tpi_set_state(tp, oldstate);
		/* The put and service procedures to not recognized all errors.  Sometimes we
		   return an error to here just to restore the previous state. */
		switch (rtn) {
		case -EBUSY:
			/* flow controlled */
		case -EAGAIN:
			/* try again */
		case -ENOMEM:
			/* could not allocate memory */
		case -ENOBUFS:
			/* could not allocate a buffer */
		case -EOPNOTSUPP:
			/* primitive not supported */
			return te_error_ack(q, prim, rtn);
		case -EPROTO:	/* terminal error */
			return te_error_reply(q, -EPROTO);
		default:
			/* ignore all other errors */
			rtn = 0;
			break;
		}
	}
	return (rtn);
}
STATIC int
tp_w_ioctl(queue_t *q, mblk_t *mp)
{
}

/*
 *  Receiving Transport Protocol messages.
 *  --------------------------------------
 */

/**
 * tp_recv_cr - Receive a Connection Request (CR) Message
 * @q: receiving queue
 * @dp: mesage block containing message
 *
 * NOTICES: The passed in data block has to be pulled up by the caller.
 */
STATIC int
tp_recv_cr(queue_t *q, register unsigned char *p, register unsigned char *e, mblk_t *dp)
{
	struct tp *tp = TP_PRIV(q);
	register unsigned char *p = b;
	unsigned short dref, sref, credit;
	uchar class, options, addopts = 0x01, altclasses = 0;

	if (e < p + 7)
		/* must include the fixed part */
		goto unspecified;
	p++;
	credit = (ushort) (*p++) & 0x0f;
	opt->rem.credit = credit;
	if (unlikely((dref = ((ushort) (*p++) << 8) | (ushort) (*p++)) != 0))
		goto invalid_parm_value;
	if (unlikely((sref = ((ushort) (*p++) << 8) | (ushort) (*p++)) == 0))
		goto invalid_parm_value;
	opt->rem.reference = sref;
	class = options = *p++;
	if ((class >>= 4) < T_CLASS4 && tp->n_type != N_CONS)
		goto invalid_parm_value;
	if (class > T_CLASS4)
		goto invalid_parm_value;
	opt->flags |= _T_BIT_TCO_PREFCLASS;
	opt->tco.tco_prefclass = class;
	altclasses = (1 << class);
	if ((options &= 0x0f) & 0x0c)
		goto invalid_parm_value;
	if (class >= T_CLASS1) {
		opt->flags |= _T_BIT_TCO_EXTFORM;
		opt->tco.tco_extform = (options & _T_F_CO_EXFORMAT) ? T_YES : T_NO;
	}
	if (class >= T_CLASS2) {
		opt->flags |= _T_BIT_TCO_FLOWCTRL;
		opt->tco.tco_flowctrl = (options & _T_F_CO_FLOWCTRL) ? T_YES : T_NO;
	}

	while (p < e) {
		uchar ptype;
		uint plen;

		if (e < p + 3)
			/* paraemters must be at least 3 bytes in length */
			goto invalid_parm_value;
		ptype = *p++;
		if ((plen = *p++) < 1)
			goto invalid_parm_value;
		if (e < p + plen)
			goto unspecified;
		switch (ptype) {
		case _TP_PT_TPDU_SIZE:	/* TPDU size */
		{
			uint order;

			if (plen != 2)
				goto invalid_parm_value;
			order = *p++;
			if (order < 7)
				goto invalid_parm_value;
			opt->tco.tco_ltpdu = (1 << (order - 7));
			opt->flags |= _T_BIT_TCO_LTPDU;
			continue;
		}
		case _TP_PT_CGTRANSSEL:	/* calling transport selector */
			opt->rem.cgtranssel.len = plen;
			bcopy(p, opt->rem.cgtranssel.val, plen);
			break;
		case _TP_PT_CDTRANSSEL:	/* called transport selector */
			opt->rem.cdtranssel.len = plen;
			bcopy(p, opt->rem.cdtranssel.val, plen);
			break;
		case _TP_PT_CHECKSUM:	/* checksum */
			if (plen != 2)
				goto invalid_parm_value;
			checksum = ((ushort) (*p++) << 8 | (ushort) (*p++));
			continue;
		case _TP_PT_VERSION:	/* version number */
			if (plen != 1)
				goto invalid_parm_value;
			opt->rem.version = *p++;
			continue;
		case _TP_PT_PROTECTION:	/* protection */
			if (plen != 1)
				goto invalid_parm_value;
			switch ((opt->tco.tco_protection = *p++)) {
			case T_NOPROTECT:
			case T_PASSIVEPROTECT:
			case T_ACTIVEPROTECT:
			case (T_PASSIVEPROTECT | T_ACTIVEPROTECT):
				break;
			default:
				goto invalid_parm_value;
			}
			opt->flags |= _T_BIT_TCO_PROTECTION;
			continue;
		case _TP_PT_ADDOPTIONS:	/* additional options */
			if (plen != 1)
				goto invalid_parm_value;
			if ((addopts = *p++) & 0x80)
				goto invalid_parm_value;
			if (class >= T_CLASS1) {
				opt->tco.tco_netexp = (addopts & _T_F_AO_NETEXP) ? T_YES : T_NO;
				opt->flags |= _T_BIT_TCO_NETEXP;
				opt->tco.tco_netrecptcf =
				    (addopts & _T_F_AO_NETRECPTCF) ? T_YES : T_NO;
				opt->flags |= _T_BIT_TCO_NETRECPTCF;
			}
			if (class >= T_CLASS4) {
				opt->tco.tco_checksum = (addopts & _T_F_AO_CHECKSUM) ? T_YES : T_NO;
				opt->flags |= _T_BIT_TCO_CHECKSUM;
			}
			opt->tco.tco_expd = (addopts & _T_F_AO_EXPD) ? T_YES : T_NO;
			opt->flags |= _T_BIT_TCO_EXPD;
			continue;
		case _TP_PT_ALTCLASS:	/* alternative protocol classes */
			if (plen > 4)
				goto invalid_parm_value;
			while (plen--) {
				uchar altclass;

				opt->flags |= (_T_BIT_TCO_ALTCLASS1 << plen);
				if ((altclass = (&opt->tco.tco_altclass1)[plen] = *p++) > T_CLASS4)
					goto invalid_parm_value;
				if (altclass < T_CLASS4 && tp->n_type != N_CONS)
					goto invalid_parm_value;
				altclasses |= (1 << altclass);
			}
			continue;
		case _TP_PT_ACKTIME:	/* acknowledgement time */
			if (plen != 2)
				goto invalid_parm_value;
			opt->tco.tco_acktime = *p++;
			opt->tco.tco_acktime <<= 8;
			opt->tco.tco_acktime |= *p++;
			opt->flags |= _T_BIT_TCO_ACKTIME;
			continue;
		case _TP_PT_RESERRORRATE:	/* residual error rate */
			if (plen != 3)
				goto invalid_parm_value;
			opt->tco.tco_reserrorrate.targetvalue = *p++;
			opt->tco.tco_reserrorrate.minacceptvalue = *p++;
			opt->rem.base_tpdu = *p++;
			opt->flags |= _T_BIT_TCO_RESERRORRATE;
			continue;
		case _TP_PT_PRIORITY:	/* priority */
		{
			ushort priority;

			if (plen != 2)
				goto invalid_parm_value;
			priority = *p++;
			priority <<= 8;
			priority |= *p++;
			opt->tco.tco_priority = priority;
			opt->flags |= _T_BIT_TCO_PRIORITY;
			continue;
		}
		case _TP_PT_TRANSDEL:	/* transit delay */
			if (plen != 8)
				goto invalid_parm_value;
			opt->tco.tco_transdel.maxdel.called.targetvalue = *p++;
			opt->tco.tco_transdel.maxdel.called.targetvalue <<= 8;
			opt->tco.tco_transdel.maxdel.called.targetvalue |= *p++;
			opt->tco.tco_transdel.maxdel.called.minacceptvalue = *p++;
			opt->tco.tco_transdel.maxdel.called.minacceptvalue <<= 8;
			opt->tco.tco_transdel.maxdel.called.minacceptvalue |= *p++;
			opt->tco.tco_transdel.maxdel.calling.targetvalue = *p++;
			opt->tco.tco_transdel.maxdel.calling.targetvalue <<= 8;
			opt->tco.tco_transdel.maxdel.calling.targetvalue |= *p++;
			opt->tco.tco_transdel.maxdel.calling.minacceptvalue = *p++;
			opt->tco.tco_transdel.maxdel.calling.minacceptvalue <<= 8;
			opt->tco.tco_transdel.maxdel.calling.minacceptvalue |= *p++;
			opt->flags |= _T_BIT_TCO_TRANSDEL;
			continue;
		case _TP_PT_THROUGHPUT:	/* throughput */
			if (plen != 12 && plen != 24)
				goto invalid_parm_value;
			opt->tco.tco_throughput.maxthrpt.called.targetvalue = *p++;
			opt->tco.tco_throughput.maxthrpt.called.targetvalue <<= 8;
			opt->tco.tco_throughput.maxthrpt.called.targetvalue |= *p++;
			opt->tco.tco_throughput.maxthrpt.called.targetvalue <<= 8;
			opt->tco.tco_throughput.maxthrpt.called.targetvalue |= *p++;
			opt->tco.tco_throughput.maxthrpt.called.minacceptvalue = *p++;
			opt->tco.tco_throughput.maxthrpt.called.minacceptvalue <<= 8;
			opt->tco.tco_throughput.maxthrpt.called.minacceptvalue |= *p++;
			opt->tco.tco_throughput.maxthrpt.called.minacceptvalue <<= 8;
			opt->tco.tco_throughput.maxthrpt.called.minacceptvalue |= *p++;
			opt->tco.tco_throughput.maxthrpt.calling.targetvalue = *p++;
			opt->tco.tco_throughput.maxthrpt.calling.targetvalue <<= 8;
			opt->tco.tco_throughput.maxthrpt.calling.targetvalue |= *p++;
			opt->tco.tco_throughput.maxthrpt.calling.targetvalue <<= 8;
			opt->tco.tco_throughput.maxthrpt.calling.targetvalue |= *p++;
			opt->tco.tco_throughput.maxthrpt.calling.minacceptvalue = *p++;
			opt->tco.tco_throughput.maxthrpt.calling.minacceptvalue <<= 8;
			opt->tco.tco_throughput.maxthrpt.calling.minacceptvalue |= *p++;
			opt->tco.tco_throughput.maxthrpt.calling.minacceptvalue <<= 8;
			opt->tco.tco_throughput.maxthrpt.calling.minacceptvalue |= *p++;
			if (plen == 24) {
				opt->tco.tco_throughput.avgthrpt.called.targetvalue = *p++;
				opt->tco.tco_throughput.avgthrpt.called.targetvalue <<= 8;
				opt->tco.tco_throughput.avgthrpt.called.targetvalue |= *p++;
				opt->tco.tco_throughput.avgthrpt.called.targetvalue <<= 8;
				opt->tco.tco_throughput.avgthrpt.called.targetvalue |= *p++;
				opt->tco.tco_throughput.avgthrpt.called.minacceptvalue = *p++;
				opt->tco.tco_throughput.avgthrpt.called.minacceptvalue <<= 8;
				opt->tco.tco_throughput.avgthrpt.called.minacceptvalue |= *p++;
				opt->tco.tco_throughput.avgthrpt.called.minacceptvalue <<= 8;
				opt->tco.tco_throughput.avgthrpt.called.minacceptvalue |= *p++;
				opt->tco.tco_throughput.avgthrpt.calling.targetvalue = *p++;
				opt->tco.tco_throughput.avgthrpt.calling.targetvalue <<= 8;
				opt->tco.tco_throughput.avgthrpt.calling.targetvalue |= *p++;
				opt->tco.tco_throughput.avgthrpt.calling.targetvalue <<= 8;
				opt->tco.tco_throughput.avgthrpt.calling.targetvalue |= *p++;
				opt->tco.tco_throughput.avgthrpt.calling.minacceptvalue = *p++;
				opt->tco.tco_throughput.avgthrpt.calling.minacceptvalue <<= 8;
				opt->tco.tco_throughput.avgthrpt.calling.minacceptvalue |= *p++;
				opt->tco.tco_throughput.avgthrpt.calling.minacceptvalue <<= 8;
				opt->tco.tco_throughput.avgthrpt.calling.minacceptvalue |= *p++;
			} else {
				opt->tco.tco_throughput.avgthrpt.called.targetvalue = T_UNSPEC;
				opt->tco.tco_throughput.avgthrpt.called.minacceptvalue = T_UNSPEC;
				opt->tco.tco_throughput.avgthrpt.calling.targetvalue = T_UNSPEC;
				opt->tco.tco_throughput.avgthrpt.calling.minacceptvalue = T_UNSPEC;
			}
			opt->flags |= _T_BIT_TCO_THROUGHPUT;
			continue;
		case _TP_PT_SUBSEQUENCE:	/* subsequence number */
			goto invalid_parm_type;
		case _TP_PT_REASTIME:	/* reassignment time */
			if (plen != 2)
				goto invalid_parm_value;
			if ((1 << class) & ~((1 << T_CLASS0) | (1 << T_CLASS2)))
				goto invalid_parm_value;
			opt->tco.tco_reastime = *p++;
			opt->tco.tco_reastime <<= 8;
			opt->tco.tco_reastime |= *p++;
			opt->flags |= _T_BIT_TCO_REASTIME;
			continue;
		case _TP_PT_FLOWCTLCF:	/* flow control confirmation */
			goto invalid_parm_type;
		case _TP_PT_SELECTACK:	/* selective acknowledgement */
			goto invalid_parm_type;
		case _TP_PT_ED_TPDU_NR:	/* ED-TPDU-NR */
			goto invalid_parm_type;
		case _TP_PT_DIAGNOSTIC:	/* diagnostic */
			goto invalid_parm_type;
		case _TP_PT_PREF_TPDU_SIZE:	/* preferred TPDU size */
		{
			register uint32_t size = 0;

			if (plen > 4)
				goto invalid_parm_value;
			while (plen--) {
				size <<= 8;
				size |= *p++;
			}
			opt->tco.tco_ltpdu = size;
			opt->flags |= _T_BIT_TCO_LTPDU;
			continue;
		}
		case _TP_PT_INACTTIME:	/* inactivity time */
			if (plen != 4)
				goto invalid_parm_value;
			if (class != T_CLASS4)
				goto invalid_parm_value;
			opt->rem.inactivity = *p++;
			opt->rem.inactivity <<= 8;
			opt->rem.inactivity |= *p++;
			opt->rem.inactivity <<= 8;
			opt->rem.inactivity |= *p++;
			opt->rem.inactivity <<= 8;
			opt->rem.inactivity |= *p++;
			/* why does not XTI know about this option? */
			continue;
		default:
			goto invalid_parm_type;
		}
		p += plen;
		continue;
	}
	return (0);
      unspecified:
	cause = _TP_ERROR_UNSPECIFIED;
	goto error;
#if 0
      invalid_tpdu_type:
	cause = _TP_ERROR_INVALID_TPDU_TYPE;
	goto error;
#endif
      invalid_parm_type:
	cause = _TP_ERROR_INVALID_PARM_TYPE;

	if (!(ep = tp_pack_er(q, qback(q), cause, b, p - b)))
		return (-ENOBUFS);
	/* fixme, need to reverse route to lower mux queue */
};

/**
 * tp_recv_cc - Receive a Connection Confirmation (CC) Message
 * @q: receiving queue
 * @p: beginning of header (including li)
 * @e: end of header
 * @dp: mesage block containing message
 */
STATIC int
tp_recv_cc(queue_t *q, unsigned char *b, register unsigned char *e, mblk_t *dp)
{
};

/**
 * tp_recv_dr - Receive a Disconnect Request (DR) Message
 * @q: receiving queue
 * @p: beginning of header (including li)
 * @e: end of header
 * @dp: mesage block containing message
 */
STATIC int
tp_recv_dr(queue_t *q, register unsigned char *p, register unsigned char *e, mblk_t *dp)
{
};

/**
 * tp_recv_dc - Receive a Diconnect Confirmation (DC) Message
 * @q: receiving queue
 * @p: beginning of header (including li)
 * @e: end of header
 * @dp: mesage block containing message
 */
STATIC int
tp_recv_dc(queue_t *q, register unsigned char *p, register unsigned char *e, mblk_t *dp)
{
};

/**
 * tp_recv_dt - Receive a Data (DT) Message
 * @q: receiving queue
 * @p: beginning of header (including li)
 * @e: end of header
 * @dp: mesage block containing message
 */
STATIC int
tp_recv_dt(queue_t *q, register unsigned char *p, register unsigned char *e, mblk_t *dp)
{
};

/**
 * tp_recv_ed - Receive a Expedited Data (ED) Message
 * @q: receiving queue
 * @p: beginning of header (including li)
 * @e: end of header
 * @dp: mesage block containing message
 */
STATIC int
tp_recv_ed(queue_t *q, register unsigned char *p, register unsigned char *e, mblk_t *dp)
{
};

/**
 * tp_recv_ak - Receive an Acknowledgement (AK) Message
 * @q: receiving queue
 * @p: beginning of header (including li)
 * @e: end of header
 * @dp: mesage block containing message
 */
STATIC int
tp_recv_ak(queue_t *q, register unsigned char *p, register unsigned char *e, mblk_t *dp)
{
};

/**
 * tp_recv_ea - Receive an Expedited Data Acknowledgement (EA) Message
 * @q: receiving queue
 * @p: beginning of header (including li)
 * @e: end of header
 * @dp: mesage block containing message
 */
STATIC int
tp_recv_ea(queue_t *q, register unsigned char *p, register unsigned char *e, mblk_t *dp)
{
};

/**
 * tp_recv_rj - Receive a Reject (RJ) Message
 * @q: receiving queue
 * @p: beginning of header (including li)
 * @e: end of header
 * @dp: mesage block containing message
 */
STATIC int
tp_recv_rj(queue_t *q, register unsigned char *p, register unsigned char *e, mblk_t *dp)
{
};

/**
 * tp_recv_er - Receive a Error (ER) Message
 * @q: receiving queue
 * @p: beginning of header (including li)
 * @e: end of header
 * @dp: mesage block containing message
 */
STATIC int
tp_recv_er(queue_t *q, register unsigned char *p, register unsigned char *e, mblk_t *dp)
{
};

/**
 * tp_recv_ud - Receive a Unit Data (UD) Message
 * @q: receiving queue
 * @p: beginning of header (including li)
 * @e: end of header
 * @dp: mesage block containing message
 */
STATIC int
tp_recv_ud(queue_t *q, register unsigned char *p, register unsigned char *e, mblk_t *dp)
{
};

/**
 * tp_recv_msg - Receive a TPDU on a stream
 * @q: receiving queue (transport read queue)
 * @dp: N_DATA_IND, N_EXDATA_IND or N_UNITDATA_IND message
 */
STATIC int
tp_recv_msg(queue_t *q, mblk_t *mp)
{
	struct tp *tp = TP_PRIV(q);
	mblk_t *dp = mp->b_cont;
	register unsigned char *p = dp->b_rptr;
	register unsigned char *e = p + p[0] + 1;
	int hlen, dlen, rtn;
	uchar mtype;

	p = mp->b_rptr;
	hlen = p[0] + 1;
	e = p + hlen;
	if ((dlen = mp->b_wptr - e) < 0)
		goto short_header;
	switch ((mtype = p[1]) & 0xf0) {
	case _TP_MT_CR:	/* CR */
		rtn = tp_recv_cr(q, p, e, mp);
		break;
	case _TP_MT_CC:	/* CC */
		rtn = tp_recv_cc(q, p, e, mp);
		break;
	case _TP_MT_DR:	/* DR */
		rtn = tp_recv_dr(q, p, e, mp);
		break;
	case _TP_MT_DC:	/* DC */
		rtn = tp_recv_dc(q, p, e, mp);
		break;
	case _TP_MT_DT:	/* DT */
		rtn = tp_recv_dt(q, p, e, mp);
		break;
	case _TP_MT_ED:	/* ED */
		rtn = tp_recv_ed(q, p, e, mp);
		break;
	case _TP_MT_AK:	/* AK */
		rtn = tp_recv_ak(q, p, e, mp);
		break;
	case _TP_MT_EA:	/* EA */
		rtn = tp_recv_ea(q, p, e, mp);
		break;
	case _TP_MT_RJ:	/* RJ */
		rtn = tp_recv_rj(q, p, e, mp);
		break;
	case _TP_MT_ER:	/* ER */
		rtn = tp_recv_er(q, p, e, mp);
		break;
	case _TP_MT_UD:	/* UD */
		rtn = tp_recv_ud(q, p, e, mp);
		break;
	default:
		goto bad_message_type;
	}
	return (rtn);
}

/*
 *  Network packet handlers
 *  =======================
 *  Rather that link any network provider streams under TP, the idea here is to use the Linux packet
 *  layer to do what we need to do, at least for TP4 CLNS.  TP4 CLNS can use UDP port 147 for
 *  ISO-grams, or IP protocol 80, or a new ETH_P_CLNL in 802.2 LLC or SNAP headers for direct LAN
 *  connections.  Therefore we need an ETH_P_CLNL packet hander and ARPHDR_CLNL handler and X.121
 *  arp tables and routing.  We need to register an inet_protocol for protocol 80.  And we need to
 *  intercept UDP PACKET_HOST packets for port 147.  We also need to change sk_buffs into mblks and
 *  visa versa.
 */

/*
 *  Zero copy conversion between sk_buffs and msgbs:
 *
 *  Converting from an sk_buff to a chain of mblks is probably pretty easy.  Use esballoc to
 *  allocate a message block for the sk_buff header, map each page in the page fragment list and
 *  esballoc a message block, hiding the vaddr in the internal data buffer somewhere, and then do
 *  the same for each sk_buff in the frag list.  Each time an mblk is esballoc'ed take another
 *  reference on the original sk_buff with skb_get() (except for the inital mblk).  When the
 *  external buffer databs are freed, each paged fragment has its vaddr unmapped and a reference
 *  released on the head sk_buff with kfree_skb(), each real memory block simply releases a
 *  reference on the head sk_buff with kfree_skb().  When the last reference to the head sk_buff is
 *  released, the head sk_buff is freed. 
 *
 *  Coversion the other way might be difficult.  The major hurdle is that the sk_buff saves shared
 *  information hidden at the end of the data buffer.  One possibilty for msgb chains is to allocate
 *  a zero-length sk_buff as a header and then place the data buffer information into the paged frag
 *  list (up to 6 buffers in the chain) and then pull some portion of the header (say the first
 *  fragment).  Then these pages could be mapped by the user of the sk_buff as required.  An
 *  skb->destructor is set that uses information in the control block.  Another wrinkle is that the
 *  control block is overwritten between layers.  This could be overcome, I suppose, by placing the
 *  sk_buff pointer in the head msblk (b_prev) and then linking them into a hash table (b_next).
 *  The destructor could then search this hash and free the corresponding mblk.
 */

/*
 *  NS-Provider Primitives received from downstream.
 *  ------------------------------------------------
 */

/**
 * ne_conn_ind - process an NE_CONN_IND event
 * @q: the lower multipelx queue on which the message arrived
 * @mp: the N_CONN_IND message
 */
STATIC int
ne_conn_ind(queue_t *q, mblk_t *mp)
{
	/* Transport providers are not interested in these, these should go to the network
	   connection management Stream. */
}

/**
 * ne_conn_con - process an NE_CONN_CON event
 * @q: the lower multipelx queue on which the message arrived
 * @mp: the N_CONN_CON message
 */
STATIC int
ne_conn_con(queue_t *q, mblk_t *mp)
{
	/* Transport provider are not interested in these, these should go to the network
	   connection management Stream. */
}

/**
 * ne_discon_ind - process an NE_DISCON_IND event
 * @q: the lower multipelx queue on which the message arrived
 * @mp: the N_DISCON_IND message
 */
STATIC int
ne_discon_ind(queue_t *q, mblk_t *mp)
{
	/* Transport providers are not directly interested in these, these should go to the network
	   connection management Stream. */
}

/**
 * ne_data_ind - process an NE_DATA_IND event
 * @q: the lower multipelx queue on which the message arrived
 * @mp: the N_DATA_IND or M_DATA message
 *
 * This function handles both M_(PC)PROTO and M_DATA messages.  These need to be routed to the
 * associated Transport Provider.
 *
 * If there is no transport endpoint attached to this network connection, the message needs to be
 * discarded or handled in some fashion.  If there is a single transport endpoint attached to this
 * network connection, the message should be passed to that upper Stream.  If there is more than one
 * transport endpoint attached to this network connection, the transport endpoint must be determined
 * by looking at the TPDU type and destination local reference contained in the TPDU.
 *
 * To make passing messages across the multiplexing driver consistent, a Network Control Block is
 * allocated in an M_(PC)CTL message block and attached to the beginning of the message.  As this is
 * an internal interface, we largely trust the Network Provider stream for proper message syntax.
 *
 * Flow Control: It is not the responsibility of the lower multiplex to perform flow control on the
 * upper multiplex.  That is the responsibilty of the upper multiplex put procedure.  We never want
 * the lower multiplex network provider to be blocked due to a flow controlled upper mutliplex.
 */
STATIC INLINE fastcall __hot_get int
ne_data_ind(queue_t *q, mblk_t *mp)
{
	struct np *np = NP_PRIV(q);
	mblk_t *cb = NULL;
	struct ncb *ncb;
	N_data_ind_t *p;
	np_ulong flags = 0;
	struct tp *tp;
	int err = -EFAULT;

	/* First, we need to aggregate together NPDUs into NSDUs. */
	p = (N_data_ind_t *) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto efault;

	flags = p->DATA_xfer_flags;

	if ((cb = ss7_allocb(q, sizeof(*ncb), BPRI_MED)) == NULL)
		goto enobufs;

	cb->b_datap->db_type = M_CTL;

	/* Routing. */
	if ((tp = np->lasttp) == NULL) {
		/* If there was a previous partial TPDU delivered, this data is delivered to the
		   same transport.  Otherwise, we need to find the upper transport Stream. */
		if (np->tp.numb <= 0) {
			/* If there are no network transport streams, the question of how to
			   respond arrises.... */
			err = QR_DONE;	/* discard for now */
			goto error;
		} else if (np->tp.numb == 1) {
			/* If there is only one transport stream, the question of demultiplexing is
			   moot.  The upper multiplex is responsible for checking all contents of
			   the message. */
			tp = np->tp.list->tp;
		} else if (np->tp.numb > 1) {
			/* If it is a connectionless TPDU, pass it to connectionless selection. */
			/* If it is a connection establishment TPDU (one with dref == 0), pass it
			   to connection establishment selection (i.e. listening streams). */
			/* If it is a connection oriented message (dref != 0), pass it to
			   connection selection (i.e. established stream). */
		} else {
			never();
		}
	}

	/* Deliver the message */

	np->lastflags |= flags;	/* don't lose receive confirmation flag */

	if (flags & N_MORE_DATA_FLAG) {
		np->lasttp = tp;
	} else {
		np->lasttp = NULL;
		/* Handled receipt confirmation. */
		if (np->lastflags & N_RC_FLAG) {
			N_datack_req_t *d;

			/* Re-use the M_PROTO message block so we don't have to worry about
			   failling to allocate blocks. */
			mp->b_cont = NULL;
			d = (N_datack_req_t *) mp->b_rptr;
			d->PRIM_type = N_DATACK_REQ;
			mp->b_wptr = mp->b_rptr + sizeof(*d);
			qreply(q, mp);
			return (QR_ABSORBED);
		}
	}
	return (QR_STRIP);
      enobufs:
	err = -ENOBUFS;
	goto error;
      error:
	if (cb)
		freemsg(cb);
	return (err);
}

/**
 * ne_exdata_ind - process an NE_EXDATA_IND event
 * @q: the lower multipelx queue on which the message arrived
 * @mp: the N_EXDATA_IND message
 */
STATIC INLINE fastcall __hot_get int
ne_exdata_ind(queue_t *q, mblk_t *mp)
{
	/* These ned to be routed to the associated Transport Provider. */
}

/**
 * ne_info_ack - process an NE_INFO_ACK event
 * @q: the lower multipelx queue on which the message arrived
 * @mp: the N_INFO_ACK message
 */
STATIC int
ne_info_ack(queue_t *q, mblk_t *mp)
{
	/* We are sometimes interested in these, sometimes it is the network connect management
	   Stream that is ultimately interested. */
}

/**
 * ne_bind_ack - process an NE_BIND_ACK event
 * @q: the lower multipelx queue on which the message arrived
 * @mp: the N_BIND_ACK message
 */
STATIC int
ne_bind_ack(queue_t *q, mblk_t *mp)
{
	/* These need to go to the Network Connection Management Stream. */
}

/**
 * ne_error_ack - process an NE_ERROR_ACK event
 * @q: the lower multipelx queue on which the message arrived
 * @mp: the N_ERROR_ACK message
 */
STATIC int
ne_error_ack(queue_t *q, mblk_t *mp)
{
	/* These need to go to the Network Connection Management Stream. */
}

/**
 * ne_ok_ack - process an NE_OK_ACK event
 * @q: the lower multipelx queue on which the message arrived
 * @mp: the N_OK_ACK message
 */
STATIC int
ne_ok_ack(queue_t *q, mblk_t *mp)
{
	/* These need to go to the Network Connection Management Stream, unless they are simply
	   acknowledging an internally generated N_OPTMGMT_REQ. */
}

/**
 * ne_unitdata_ind - process an NE_UNITDATA_IND event
 * @q: the lower multipelx queue on which the message arrived
 * @mp: the N_UNITDATA_IND message
 */
STATIC INLINE fastcall __hot_get int
ne_unitdata_ind(queue_t *q, mblk_t *mp)
{
	/* These need to be routed to the associated COTS Class 4 over CONS Transport Provider. */
}

/**
 * ne_uderror_ind - process an NE_UDERROR_IND event
 * @q: the lower multipelx queue on which the message arrived
 * @mp: the N_UDERROR_IND message
 */
STATIC INLINE fastcall __hot_get int
ne_uderror_ind(queue_t *q, mblk_t *mp)
{
	/* Don't know what to do with these. */
}

/**
 * ne_datack_ind - process an NE_DATACK_IND event
 * @q: the lower multipelx queue on which the message arrived
 * @mp: the N_DATACK_IND message
 */
STATIC INLINE fastcall __hot_get int
ne_datack_ind(queue_t *q, mblk_t *mp)
{
	/* Don't know what to do with these. */
}

/**
 * ne_reset_ind - process an NE_RESET_IND event
 * @q: the lower multipelx queue on which the message arrived
 * @mp: the N_RESET_IND message
 */
STATIC int
ne_reset_ind(queue_t *q, mblk_t *mp)
{
	/* These are processed internally or go to the Network Connection Management Stream. */
}

/**
 * ne_reset_con - process an NE_RESET_CON event
 * @q: the lower multipelx queue on which the message arrived
 * @mp: the N_RESET_CON message
 */
STATIC int
ne_reset_con(queue_t *q, mblk_t *mp)
{
	/* Don't know what to do with these.  TP never requests a reset so there should be no
	   confirmation.  Perhaps they should go to the Network Connection Mangement Stream. */
}

STATIC INLINE fastcall __hot_get int
np_r_data(queue_t *q, mblk_t *mp)
{
	return ne_data_ind(q, mp);
}
STATIC INLINE fastcall __hot_get int
np_r_proto(queue_t *q, mblk_t *mp)
{
	int rtn = -EPROTO;
	np_ulong prim = 0;
	struct np *np = NP_PRIV(q);
	np_ulong oldstate = npi_get_state(np);

	if (mp->b_wptr >= mp->b_rptr + sizeof(prim)) {
		switch ((prim = *((np_ulong *) mp->b_rptr))) {
		case N_CONN_IND:
			rtn = ne_conn_ind(q, mp);
			break;
		case N_CONN_CON:
			rtn = ne_conn_con(q, mp);
			break;
		case N_DISCON_IND:
			rtn = ne_discon_ind(q, mp);
			break;
		case N_DATA_IND:
			rtn = ne_data_ind(q, mp);
			break;
		case N_EXDATA_IND:
			rtn = ne_exdata_ind(q, mp);
			break;
		case N_INFO_ACK:
			rtn = ne_info_ack(q, mp);
			break;
		case N_BIND_ACK:
			rtn = ne_bind_ack(q, mp);
			break;
		case N_ERROR_ACK:
			rtn = ne_error_ack(q, mp);
			break;
		case N_OK_ACK:
			rtn = ne_ok_ack(q, mp);
			break;
		case N_UNITDATA_IND:
			rtn = ne_unitdata_ind(q, mp);
			break;
		case N_UDERROR_IND:
			rtn = ne_uderror_ind(q, mp);
			break;
		case N_DATACK_IND:
			rtn = ne_datack_ind(q, mp);
			break;
		case N_RESET_IND:
			rtn = ne_reset_ind(q, mp);
			break;
		case N_RESET_CON:
			rtn = ne_reset_con(q, mp);
			break;
		case N_UNITDATA_REQ:
		case N_DATA_REQ:
		case N_CONN_REQ:
		case N_CONN_RES:
		case N_DISCON_REQ:
		case N_EXDATA_REQ:
		case N_DATACK_REQ:
		case N_RESET_REQ:
		case N_RESET_RES:
		case N_INFO_REQ:
		case N_BIND_REQ:
		case N_UNBIND_REQ:
		case N_OPTMGMT_REQ:
			/* wrong direction */
			rtn = -EPROTO;
			break;
		default:
			rtn = -EOPNOTSUPP;
			break;
		}
	}
	if (rtn < 0) {
		seldom();
		npi_set_state(np, oldstate);
		/* The put and service procedures do not recognize all errors.  Sometimes we return
		   an error to here just to restore the previous state. */
		switch (rtn) {
		case -EBUSY:
		case -EAGAIN:
		case -ENOMEM:
		case -ENOBUFS:
		case -EOPNOTSUPP:
			return ne_error_ind(q, prim, rtn);
		case -EPROTO:
			return ne_error_reply(q, -EPROTO);
		default:
			rtn = 0;
			break;
		}
	}
	return (rtn);
}
STATIC INLINE fastcall int
np_r_error(queue_t *q, mblk_t *mp)
{
}
STATIC INLINE fastcall int
np_r_hangup(queue_t *q, mblk_t *mp)
{
}
STATIC INLINE fastcall int
np_r_unhangup(queue_t *q, mblk_t *mp)
{
}

/*
 *  Put and Service procedures.
 *  ===========================
 */
/**
 * tp_r_wakeup - wake up the read queue
 * @q: the read queue to wake up
 *
 * This function serves to enable feeding lower multiplex Streams across the multiplexing driver
 * when the upper multiplex Stream is no longer flow controlled.  This is similar to the actions
 * performed by the Driver read queue.  Note that it is never necessary to place message on the
 * upper read queue.  If the upper read queue fails a bcanputnext(), the message is placed back on
 * the feeding lower read queue.  When congestion upstream of the upper multiplex Stream abates, the
 * upper multiplex read service procedure will be backenabled and this wakeup function called.
 *
 * Note that the lower read queue might assume a different policy depending on the protocol class.
 * That is, if a Network Service Provider lower Stream is feeding multiple upper Transport Service
 * Provider Streams, it might choose to discard the message rather than queuing it and propagating
 * the congestion to other upper Transport Service Provider Streams.
 */
STATIC void
tp_r_wakeup(queue_t *q)
{
	/* When this module is pushed over an NS-Provider stream, it is not necessary to backenable 
	   the preceding queue as it will be backenabled automatically by STREAMS. When this upper
	   multiplex Stream is linked only to one lower multiplex, it might be an idea to just weld 
	   the queues together, otherwise, the multiple feed logic below will work ok. */
	if (WR(q)->q_next == NULL) {
		struct tp *tp = TP_PRIV(q);
		struct np *np;
		unsigned long flags;

		/* When this upper multiplex Stream is linked to lower multiplex Streams, it is
		   necessary to manually enable the feeding queues accross the multiplex. */

		spin_lock_str(&tp->lock, flags);
		for (np = tp->np.np; np; np = np->tp.next)
			qenable(np->iq);
		spin_unlock_str(&tp->lock, flags);
	}
}
STATIC int
tp_r_prim(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_HANGUP:
		return tp_r_hangup(q, mp);
	case M_ERROR:
		return tp_r_error(q, mp);
	case M_FLUSH:
		return ss7_r_flush(q, mp);
	}
	return (QR_PASSFLOW);
}
STATIC int
tp_w_prim(queue_t *q, mblk_t *mp)
{
	/* Fast Path */
	if (mp->b_datap->db_type == M_DATA)
		return tp_w_data(q, mp);
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return tp_w_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return tp_w_proto(q, mp);
	case M_FLUSH:
		return ss7_w_flush(q, mp);
	case M_IOCTL:
		return tp_w_ioctl(q, mp);
	}
	seldom();
	return (QR_PASSFLOW);
}
STATIC int
np_r_prim(queue_t *q, mblk_t *mp)
{
	/* Fast Path */
	if (mp->b_datap->db_type == M_DATA)
		return np_r_data(q, mp);
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return np_r_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return np_r_proto(q, mp);
	case M_FLUSH:
		return ss7_r_flush(q, mp);
	case M_ERROR:
		return np_r_error(q, mp);
	case M_HANGUP:
		return np_r_hangup(q, mp);
#ifdef M_UNHANGUP
	case M_UNHANGUP:
		return np_r_unhangup(q, mp);
#endif
	}
	seldom();
	return (QR_PASSFLOW);
}
STATIC int
np_w_prim(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_FLUSH:
		return ss7_w_flush(q, mp);
	}
	return (QR_PASSFLOW);
}

/**
 * np_w_wakeup - wake up the write queue
 * @q: the write queue to wake up
 *
 * This function serves to enable feeding upper multiplex Streams across the multiplexing driver
 * when the lower multiplex Stream is no longer flow controlled.  This is similar to actions
 * performed by the Stream head write queue.  Note that it is never necessary to place messages on
 * the lower write queue.  If the lower write queue fails a bcanputnext(), the message is placed
 * back on the feeding upper write queue.  When congestion downstream of the lower multiplex Stream
 * abates, the lower multiplex write service procedure will be backenabled and this wakeup function
 * called.
 */
STATIC void
np_w_wakeup(queue_t *q)
{
	/* When this modules is pushed over an NS-Provider stream, this function is never invoked
	   because it belongs only to the lower multiplex.  When this lower multiplex Stream is
	   linked to only one upper multiplex, it might be an idea to just weld the queues
	   together, otherwise, the multiple feed logic below will work ok. */
	if (RD(q)->q_next == NULL) {
		struct np *np = NP_PRIV(q);
		struct tp *tp;
		unsigned long flags;

		/* When this lower multiplex Stream is linked to upper multiplex Streams, it is
		   necessary to manually enable the feeding queues across the multiplex. */

		spin_lock_str(&np->lock, flags);
		for (tp = np->tp.tp; tp; tp = tp->np.next)
			qenable(tp->iq);
		spin_unlock_str(&np->lock, flags);
	}
}

/*
 *  Private Structure allocation, deallocation and cache
 *  ====================================================
 */
STATIC int
tp_term_caches(void)
{
	if (tp_priv_cachep != NULL) {
#ifdef HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(tp_priv_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy tp_priv_cachep", __FUNCTION__);
			return (-EBUSY);
		}
#else
		kmem_cache_destroy(tp_priv_cachep);
#endif
		printd(("%s: destroyed tp_priv_cachep\n", DRV_NAME));
	}
	if (tp_link_cachep != NULL) {
#ifdef HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(tp_link_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy tp_link_cachep", __FUNCTION__);
			return (-EBUSY);
		}
#else
		kmem_cache_destroy(tp_link_cachep);
#endif
		printd(("%s: destroyed tp_link_cachep\n", DRV_NAME));
	}
	return (0);
}
STATIC int
tp_init_caches(void)
{
	if (tp_priv_cachep == NULL) {
		tp_priv_cachep = kmem_create_cache("tp_priv_cachep", sizeof(tp_t), 0,
						   SLAB_HWCACHE_ALIGN, NULL, NULL);
		if (tp_priv_cachep == NULL) {
			cmn_err(CE_PANIC, "%s: Cannot allocate tp_priv_cachep", __FUNCTION__);
			tp_term_caches();
			return (-ENOMEM);
		}
		printd(("%s: initialized driver private structure cache\n", DRV_NAME));
	}
	if (tp_link_cachep == NULL) {
		tp_link_cachep = kmem_create_cache("tp_link_cachep", sizeof(tp_t), 0,
						   SLAB_HWCACHE_ALIGN, NULL, NULL);
		if (tp_link_cachep == NULL) {
			cmn_err(CE_PANIC, "%s: Cannot allocate tp_link_cachep", __FUNCTION__);
			tp_term_caches();
			return (-ENOMEM);
		}
		printd(("%s: initialized driver link structure cache\n", DRV_NAME));
	}
	return (0);
}

/**
 * tp_alloc_priv: = allocate a private structure for the open routine
 * @q: read queue of newly created Stream
 * @tpp: point to position in list for insertion
 * @type: class of provider
 * @devp: pointer to device number
 * @crp: pointer to credentials
 *
 * Allocates a new private structure, initialized it to appropriate values, and then inserts it into
 * the private struture list.  This function must be called with the private structure list locked
 * for write and a valid @tpp pointer into the list.
 */
STATIC struct tp *
tp_alloc_priv(queue_t *q, struct tp **tpp, int type, dev_t *devp, cred_t *crp)
{
	struct tp *tp;

	if ((tp = tp_alloc())) {
		/* tp specific members */
		tp->links.prev = &tp->links.next;
		tp->links.next = NULL;
		tp->links.np = NULL;
		switch (type) {
		case TP0_CMINOR:
			tp->tp_options.tco.tco_prefclass = 0;
			tp->n_prov = N_CONS;
			break;
		case TP1_CMINOR:
			tp->tp_options.tco.tco_prefclass = 1;
			tp->n_prov = N_CONS;
			break;
		case TP2_CMINOR:
			tp->tp_options.tco.tco_prefclass = 2;
			tp->n_prov = N_CONS;
			break;
		case TP3_CMINOR:
			tp->tp_options.tco.tco_prefclass = 3;
			tp->n_prov = N_CONS;
			break;
		case TP4_CONS_CMINOR:
			tp->tp_options.tco.tco_prefclass = 4;
			tp->n_prov = N_CONS;
			break;
		case TP4_CLNS_CMINOR:
			tp->tp_options.tco.tco_prefclass = 4;
			tp->n_prov = N_CLNS;
			break;
		case NCMP_CMINOR:
			tp->tp_options.tco.tco_prefclass = -1;
			tp->n_prov = N_CLNS;
			break;
		default:
			tp_release(&tp);
			return (tp);
		}
		/* generic members */
		tp->u.dev.cmajor = getmajor(*devp);
		tp->u.dev.cminor = getminor(*devp);
		tp->cred = *crp;
		(tp->oq = q)->q_ptr = tp_get(tp);
		(tp->iq = WR(q))->q_ptr = tp_get(tp);
		tp->i_prim = &tp_w_prim;
		tp->o_prim = &tp_r_prim;
		tp->i_wakeup = NULL;
		tp->o_wakeup = &tp_r_wakeup;
		spin_lock_init(&tp->qlock);	/* "tp-queue-lock" */
		tp->i_state = LMI_UNUSABLE;
		tp->i_style = LMI_STYLE1;
		tp->i_version = 1;
	      tp->i_oldstate = LMI_UNUSABLE:
		/* link into master list */
		if ((tp->next = *tpp))
			tp->next->prev = &tp->next;
		tp->prev = tpp;
		*tpp = tp_get(tp);
	} else
		ptrace(("%s: ERROR: Could not allocate module private structure\n", DRV_NAME));
	return (tp);
}

/**
 * tp_free_priv - deallocate a private structure for the close routine
 * @q: read queue of closing Stream
 */
STATIC void
tp_free_priv(queue_t *q)
{
	struct np *np;

	ensure(q, return);
	tp = TP_PRIV(q);
	ensure(tp, return);

	/* 
	   FIXME: Here we need to handle the disposition of the state of the Stream.  If the state
	   is not unbound, we need to take it there, and transmit whatever is necessary under the
	   protocol.  Also, if there were temporarily linked N_CONS connections that this Stream
	   was using, they should have been unlinked by now.  If there are system N_CONS
	   connections that are being used, of an N_CLNS stream being use, those streams need to be 
	   detached from the Transport stream now.  If the last reference to thw lower N_CONS or
	   N_CLNS stream is released, we might consider disconnecting (and possibly unbinding) that
	   connection.

	   If we were I_PUSH'ed over the N_CONS stream, (and so now we are being I_POP'ed), the
	   application is responsible for the N_CONS stream and its state.

	 */

	qprocsoff(q);
	printd(("%s: unlinking private structure, reference count = %d\n", DRV_NAME,
		atomic_read(&tp->refcnt)));
	ss7_unbufcall((str_t *) tp);
	printd(("%s: removed bufcalls, reference count = %d\n", DRV_NAME,
		atomic_read(&tp->refcnt)));
	/* remove from master list */
	write_lock_bh(&master.lock);
	{
		if ((*tp->prev = tp->next))
			tp->next->prev = tp->prev;
		tp->next = NULL;
		tp->prev = &tp->next;
	}
	write_unlock_bh(&master.lock);
	printd(("%s: unlinked, reference count = %d\n", DRV_NAME, atomic_read(&tp->refcnt)));
	tp_release(&tp->oq->q_ptr);
	tp->oq = NULL;
	tp_release(&tp->iq->q_ptr);
	tp->iq = NULL;
	assure(atomic_read(&tp->refcnt) == 1);
	tp_release(&tp);	/* should normally be final put */
	return;
}

/**
 * tp_alloc_link - allocate a private structure for the I_LINK/I_PLINK procedure
 * @q: write queue of linked Stream
 * @npp: pointer to position in list for insertion
 * @type: permanent(1) or temporary(0)
 * @index: link id
 * @crp: pointer to credentials
 */
STATIC struct np *
tp_alloc_link(queue_t *q, struct np **npp, int type, long index, cred_t *crp)
{
	struct np *np;

	if ((np = np_alloc())) {
		/* np specific members */
		np->opens.prev = &np->opens.next;
		np->opens.next = NULL;
		np->opens.tp = NULL;
		/* generic members */
		np->u.mux.index = index;
		np->cred = *crp;
		spin_lock_init(&np->qlock);	/* "np-queue-lock" */
		(np->iq = RD(q))->q_ptr = np_get(np);
		(np->oq = WR(q))->q_ptr = np_get(np);
		np->o_prim = &np_w_prim;
		np->i_prim = &np_r_prim;
		np->o_wakeup = &np_w_wakeup;
		np->i_wakeup = NULL;
		np->i_state = LMI_UNUSABLE;
		np->i_style = LMI_STYLE1;
		np->i_version = 1;
	      np->i_oldstate = LMI_UNUSABLE:
		/* decide what list to add it to */
		switch (type) {
		case I_PLINK:
			/* permanent linkage: add it to the lower links list */
		case I_LINK:
			/* temporary linkage: add it to the stream's links list */
		default:
		}
		/* place in master list */
		if ((np->next = *npp))
			np->next->prev = &np->next;
		np->prev = npp;
		*npp = np_get(np);
		master.np.numb++;
	} else
		ptrace(("%s: ERROR: Could not allocate module linked structure\n", DRV_NAME));
	return (np);
}

STATIC void
tp_free_link(queue_t *q)
{
	struct np *np = NP_PRIV(q);

	printd(("%s: %s; %p free tp index = %lu\n", DRV_NAME, __FUNCTION__, np, np->u.mux.index));
	/* flushing buffers */
	ss7_unbufcall((str_t *) np);
	flushq(np->iq, FLUSHALL);
	flushq(np->oq, FLUSHALL);
	/* FIXME: unlink from transport providers */
	/* remove from queues */
	np_release(&np->iq->q_ptr);
	np_release(&np->oq->q_ptr);
	/* remove from master list */
	if ((*np->prev = np->next))
		np->next->prev = np->prev;
	np->next = NULL;
	np->prev = &np->next;
	np_put(np);
	master.np.numb--;
	np_release(&np);	/* final put */
}

/*
 *  Open and Close.
 *  ===============
 */

#define FIRST_CMINOR		0
#define TP0_CMINOR		0
#define TP1_CMINOR		1
#define TP2_CMINOR		2
#define TP3_CMINOR		3
#define TP4_CONS_CMINOR		4
#define TP4_CLNS_CMINOR		5
#define NCMP_CMINOR		6
#define LAST_CMINOR		6
#define FREE_CMINOR		7
STATIC int tp_majors[TP_CMAJORS] = { TP_CMAJOR_0, };

/**
 * tp_qopen: = TPI TP driver STREAMS open routine.
 * @q: read queue of opened Stream
 * @devp: pointer to device number opened
 * @oflags: flags to the open call
 * @sflag: STREAMS flag: DRVOPEN, MODOPEN or CLONEOPEN
 * @crp: pointer to opener's credentials
 *
 * This driver support all forms of open, both driver opening and module pushing.
 *
 * When the driver is opened, there are a number of distinguished clone channel minor device
 * numbers as follows:
 *
 * (major, 0) - /dev/tp0      - Transport Protocol Class 0
 * (major, 1) - /dev/tp1      - Transport Protocol Class 1
 * (major, 2) - /dev/tp2      - Transport Protocol Class 2
 * (major, 3) - /dev/tp3      - Transport Protocol Class 3
 * (major, 4) - /dev/tp4_cons - Transport Protocol Class 4 (CONS)
 * (major, 5) - /dev/tp4_clns - Transport Protocol Class 4 (CLNS)
 * (major, 6) - /dev/ncmp     - Network Connection Management Protocol (NCMP)
 */
STATIC streamscall int
tp_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	int mindex = 0;
	int type = 0;
	major_t cmajor = getmajor(*devp);
	minor_t cminor = getminor(*devp);
	struct tp *tp, **tpp;

	if (q->q_ptr != NULL) {
		return (0);	/* already open */
	}
	switch (sflag) {
	case DRVOPEN:
	case CLONEOPEN:
	{
	      drvopen:
#ifdef LIS
		if (cmajor != TP_CMAJOR_0) {
			return (ENXIO);
		}
#endif				/* LIS */
#ifdef LFS
		/* Linux Fast-STREAMS always passes internal major device numbers (modules ids) */
		if (cmajor != TP_DRV_ID) {
			return (ENXIO);
		}
#endif				/* LFS */
		if (cminor > LAST_CMINOR) {
			return (ENXIO);
		}
		type = cminor;
#if 0
		if (slfag == CLONEOPEN)
#endif
			cminor = FREE_MINOR;
		write_lock_bh(&master.lock);
		for (tpp = &master.drv.list; *tpp; tpp = &(*tpp)->next) {
			if (cmajor != (*tpp)->u.dev.cmajor)
				break;
			if (cmajor == (*tpp)->u.dev.cmajor) {
				if (cminor < (*tpp)->u.dev.cminor) {
					if (++cminor >= NMINORS) {
						if (++mindex >= TP_CMAJORS
						    || !(cmajor = tp_majors[mindex]))
							break;
						cminor = 0;
					}
					continue;
				}
			}
		}
		if (mindex >= TP_CMAJORS || !cmajor) {
			ptrace(("%s: ERROR: no device numbers available\n", DRV_NAME));
			write_unlock_bh(&master.lock);
			return (ENXIO);
		}
		printd(("%s: opened character device %d:%d\n", DRV_NAME, cmajor, cminor));
		*devp = makedevice(cmajor, cminor);
		break;
	}
	case MODOPEN:
	{
	      modopen:
		tpp = &master.mod.list;
		write_lock_bh(&master.lock);
		break;
	}
	default:
		if (WR(q)->q_next != NULL)
			goto modopen;
		goto drvopen;
	}
	if (!(tp = tp_alloc_priv(q, tpp, type, devp, crp))) {
		ptrace(("%s: ERROR: No memory\n", DRV_NAME));
		write_unlock_bh(&master.lock);
		return (ENOMEM);
	}
	write_unlock_bh(&master.lock);
	qprocson(q);
	return (0);
}

STATIC streamscall int
tp_qclose(queue_t *q, int oflags, cred_t *crp)
{
	struct tp *tp = TP_PRIV(q);

	(void) oflag;
	(void) crp;
	(void) tp;
	printd(("%s: closing character device %d:%d\n", DRV_NAME, tp->u.dev.cmajor,
		tp->u.dev.cminor));
#if defined LIS
	/* protect against LiS bugs */
	if (q->q_ptr == NULL) {
		cmn_err(CE_WARN, "%s: %s: LiS double-close bug detected.", DRV_NAME, __FUNCTION__);
		goto quit;
	}
	if (q->q_next == NULL) {
		cmn_err(CE_WARN, "%s: %s: LiS pipe bug: called with NULL q->q_next pointer",
			DRV_NAME, __FUNCTION__);
		goto skip_pop;
	}
#endif				/* defined LIS */
	goto skip_pop;
      skip_pop:
	qprocsoff(q);
	tp_free_priv(q);
	goto quit;
      quit:
	return (0);

}

STATIC streamscall int
tp_qlink(queue_t *q, long index, int lflag, cred_t *crp)
{
}

STATIC streamscall int
tp_qunlink(queue_t *q, long index, int lflag, cred_t *crp)
{
}
