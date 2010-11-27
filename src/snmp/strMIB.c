/*****************************************************************************

 @(#) $RCSfile: strMIB.c,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:42:35 $

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2010  Monavacon Limited <http://www.monavacon.com/>
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

 Last Modified $Date: 2009-06-21 11:42:35 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: strMIB.c,v $
 Revision 1.1.2.1  2009-06-21 11:42:35  brian
 - added files to new distro

 Revision 0.9.2.32  2009-02-18 20:19:01  brian
 - updated agents

 Revision 0.9.2.31  2009-01-19 10:39:41  brian
 - added new standalone agent

 Revision 0.9.2.30  2009-01-16 20:25:49  brian
 - working up streams mibs and agents

 Revision 0.9.2.29  2009-01-16 14:06:59  brian
 - update STREAMS mib

 Revision 0.9.2.28  2009-01-15 17:50:29  brian
 - working module tables

 Revision 0.9.2.27  2009-01-14 14:36:18  brian
 - latest script updates

 Revision 0.9.2.26  2009-01-14 14:32:00  brian
 - working up agents

 Revision 0.9.2.25  2009-01-10 17:19:50  brian
 - updated agents

 Revision 0.9.2.24  2009-01-04 13:16:13  brian
 - updated agents

 Revision 0.9.2.23  2009-01-03 10:51:13  brian
 - updated agent

 Revision 0.9.2.22  2009-01-02 15:46:46  brian
 - corrections

 Revision 0.9.2.21  2009-01-02 15:04:19  brian
 - updated agents

 Revision 0.9.2.20  2008-12-31 16:04:53  brian
 - updated mibs

 Revision 0.9.2.19  2008-12-24 13:42:15  brian
 - updated mibs

 Revision 0.9.2.18  2008-10-12 03:07:41  brian
 - changes for FC9 2.6.25 kernel

 Revision 0.9.2.17  2008-10-07 19:04:22  brian
 - strStrhold is a TruthValue

 Revision 0.9.2.16  2008-09-10 03:49:46  brian
 - changes to accomodate FC9, SUSE 11.0 and Ubuntu 8.04

 Revision 0.9.2.15  2008/07/28 05:14:08  brian
 - updates to mib builder and corrections to agent

 Revision 0.9.2.14  2008-04-28 12:54:07  brian
 - update file headers for release

 Revision 0.9.2.13  2008/01/14 18:04:58  brian
 - update tests and MIBS

 Revision 0.9.2.12  2008/01/13 21:53:08  brian
 - dlmod SNMP agent build and installation

 Revision 0.9.2.11  2008/01/13 00:27:44  brian
 - update mib

 Revision 0.9.2.10  2008/01/03 11:13:59  brian
 - compile and index updates

 Revision 0.9.2.9  2007/12/29 23:27:41  brian
 - updated mib

 Revision 0.9.2.8  2007/12/29 23:01:51  brian
 - working up mib2c

 Revision 0.9.2.7  2007/12/15 20:20:08  brian
 - updates

 Revision 0.9.2.6  2007/10/18 06:53:53  brian
 - STREAMS MIB updates

 Revision 0.9.2.5  2007/10/15 19:02:42  brian
 - net-snmp build corrections

 Revision 0.9.2.4  2007/10/15 17:21:27  brian
 - SNMP updates

 Revision 0.9.2.3  2007/10/15 06:47:50  brian
 - update to SNMP agent build

 Revision 0.9.2.2  2007/10/15 01:04:51  brian
 - updated SNMP build

 Revision 0.9.2.1  2007/10/13 08:54:36  brian
 - added MIB agent files

 *****************************************************************************/

static char const ident[] = "$RCSfile: strMIB.c,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:42:35 $";

/* This file was generated by mib2c and is intended for use as
   a mib module for the ucd-snmp snmpd agent. */
#include <ucd-snmp/ucd-snmp-config.h>
#include <ucd-snmp/ucd-snmp-includes.h>
#include <ucd-snmp/ucd-snmp-agent-includes.h>
#include <ucd-snmp/agent_trap.h>
#include <ucd-snmp/callback.h>
#include <ucd-snmp/snmp-tc.h>
#include <ucd-snmp/default_store.h>
#include <ucd-snmp/snmp_alarm.h>
/* The following header files are mangled in most recent net-snmp releases so
 * the versions from UCD-SNMP 4.2.5 are included here.  */
#if defined HAVE_LIBNETSNMP
#else				/* defined HAVE_LIBNETSNMP */
#endif				/* defined HAVE_LIBNETSNMP */
/* These are messed up on both. */
#include "ds_agent.h"
#ifdef HAVE_UCD_SNMP_UTIL_FUNCS_H
#include <ucd-snmp/util_funcs.h>
/* Many recent net-snmp UCD compatible headers do not declare header_generic. */
int header_generic(struct variable *, oid *, size_t *, int, size_t *, WriteMethod **);
#else				/* HAVE_UCD_SNMP_UTIL_FUNCS_H */
#include "util_funcs.h"
#endif				/* HAVE_UCD_SNMP_UTIL_FUNCS_H */
#ifdef HAVE_UCD_SNMP_HEADER_COMPLEX_H
#include <ucd-snmp/header_complex.h>
#else				/* HAVE_UCD_SNMP_HEADER_COMPLEX_H */
#include "header_complex.h"
#endif				/* HAVE_UCD_SNMP_HEADER_COMPLEX_H */
/* This one is the other way around: it is just fine for net-snmp, but
 * ucd-snmp does not provide the header file at all.  */
#ifdef HAVE_UCD_SNMP_MIB_MODULES_H
#include <ucd-snmp/mib_modules.h>
#else				/* HAVE_UCD_SNMP_MIB_MODULES_H */
#ifdef HAVE_NET_SNMP_AGENT_MIB_MODULES_H
#include <net-snmp/agent/mib_modules.h>
#else				/* HAVE_NET_SNMP_AGENT_MIB_MODULES_H */
#include "mib_modules.h"
#endif				/* HAVE_NET_SNMP_AGENT_MIB_MODULES_H */
#endif				/* HAVE_UCD_SNMP_MIB_MODULES_H */
#include <stdint.h>
#include <signal.h>
#include <sys/stat.h>		/* for struct stat, fstat() */
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <pwd.h>		/* for getpwuid() getpwnam() */
#include <grp.h>		/* for getgrgid() getgrnam() */
#include <libgen.h>		/* for basename() */
#include <fcntl.h>		/* for O_CREAT */
#include <sys/sysctl.h>		/* for sysctl */
#include <errno.h>
#include <string.h>
#ifdef _GNU_SOURCE
#include <getopt.h>
#endif
#include <limits.h>
#include <stropts.h>
#include <sys/sc.h>
#include <sys/sad.h>
#include "strExtMIB.h"
#include "strMIB.h"
#define MASTER 1
#define MY_FACILITY(__pri)	(LOG_DAEMON|(__pri))
#if defined MODULE
#if defined MASTER
const char sa_program[] = "strMIB";
int sa_fclose = 1;			/* default close files between requests */
int sa_changed = 1;			/* indication to reread MIB configuration */
int sa_stats_refresh = 1;		/* indications that statistics, the mib or its tables need to be refreshed */
int sa_request = 1;			/* request number for per-request actions */
int sa_dump = 0;			/* default packet dump */
int sa_debug = 0;			/* default no debug */
#endif				/* defined MASTER */
#endif				/* defined MODULE */
static int my_fd = -1;			/* file descriptor for this MIB's use */
static int my_readfd = -1;		/* file descriptor for autonomnous events */
volatile int strMIB_refresh = 1;
volatile int strModTable_refresh = 1;
volatile int strModInfoTable_refresh = 1;
volatile int strModStatTable_refresh = 1;
volatile int strApshTable_refresh = 1;
volatile int strStatsTable_refresh = 1;

/* /proc/streams */
enum {
	CTL_STREAMS = 11,		/* STREAMS */
};

/* /proc/sys/streams */
enum {
	STREAMS_MAXPSZ = 1,
	STREAMS_MINPSZ = 2,
	STREAMS_HIWAT = 3,
	STREAMS_LOWAT = 4,
	STREAMS_CLTIME = 5,
	STREAMS_RTIME = 6,
	STREAMS_IOCTIME = 7,
	STREAMS_NSTRPUSH = 8,
	STREAMS_STRTHRESH = 9,
	STREAMS_STRHOLD = 10,
	STREAMS_STRCTLSZ = 11,
	STREAMS_STRMSGSZ = 12,
	STREAMS_NSTRMSGS = 13,
	STREAMS_NBAND = 14,
	STREAMS_REUSE_FMODSW = 15,
	STREAMS_MAX_APUSH = 16,
	STREAMS_MAX_STRAMOD = 17,
	STREAMS_MAX_STRDEV = 18,
	STREAMS_MAX_STRMOD = 19,
	STREAMS_MAX_MBLK = 20,
	STREAMS_MSG_PRIORITY = 21,
};

/* name conflict */
#undef STRMAXPSZ
#undef STRMINPSZ

/*
 * strMIB_variables_oid: object identifier for strMIB
 * This is the top level oid that we want to register under.  This is essentially a prefix, with the
 * suffix appearing in the variable below.
 */
oid strMIB_variables_oid[9] = { 1, 3, 6, 1, 4, 1, 29591, 1, 1 };
oid strModTable_variables_oid[14] = { 1, 3, 6, 1, 4, 1, 29591, 1, 1, 1, 1, 1, 1, 1 };
oid strApshTable_variables_oid[14] = { 1, 3, 6, 1, 4, 1, 29591, 1, 1, 1, 1, 4, 1, 1 };

/*
 * Oids for use in notifications defined in this MIB.
 */

/*
 * Oids accessible only for notify defined in this MIB.
 */

/*
 * Other oids defined in this MIB.
 */
oid strMIBCompliance_oid[12] = { 1, 3, 6, 1, 4, 1, 29591, 1, 1, 2, 1, 1 };
oid strTunablesMandatoryGroup_oid[12] = { 1, 3, 6, 1, 4, 1, 29591, 1, 1, 2, 2, 1 };
oid strMessagePoolsConditionalGroup_oid[12] = { 1, 3, 6, 1, 4, 1, 29591, 1, 1, 2, 2, 2 };
oid strHoldConditionalGroup_oid[12] = { 1, 3, 6, 1, 4, 1, 29591, 1, 1, 2, 2, 3 };
oid strTunablesOptionalGroup_oid[12] = { 1, 3, 6, 1, 4, 1, 29591, 1, 1, 2, 2, 4 };
oid strApshTableMandatoryGroup_oid[12] = { 1, 3, 6, 1, 4, 1, 29591, 1, 1, 2, 2, 5 };
oid strApshTableConditionalGroup_oid[12] = { 1, 3, 6, 1, 4, 1, 29591, 1, 1, 2, 2, 6 };
oid strModTableMandatoryGroup_oid[12] = { 1, 3, 6, 1, 4, 1, 29591, 1, 1, 2, 2, 7 };
oid strModTableConditionalGroup_oid[12] = { 1, 3, 6, 1, 4, 1, 29591, 1, 1, 2, 2, 8 };
oid strModInfoTableMandatoryGroup_oid[12] = { 1, 3, 6, 1, 4, 1, 29591, 1, 1, 2, 2, 9 };
oid strModInfoTableConditionalGroup_oid[12] = { 1, 3, 6, 1, 4, 1, 29591, 1, 1, 2, 2, 10 };
oid strModStatTableMandatoryGroup_oid[12] = { 1, 3, 6, 1, 4, 1, 29591, 1, 1, 2, 2, 11 };
oid strModStatTableConditionalGroup_oid[12] = { 1, 3, 6, 1, 4, 1, 29591, 1, 1, 2, 2, 12 };
oid strStatsTableMandatoryGroup_oid[12] = { 1, 3, 6, 1, 4, 1, 29591, 1, 1, 2, 2, 13 };
oid strStatsTableConditionalGroup_oid[12] = { 1, 3, 6, 1, 4, 1, 29591, 1, 1, 2, 2, 14 };
static oid zeroDotZero_oid[2] = { 0, 0 };
static oid snmpTrapOID_oid[11] = { 1, 3, 6, 1, 6, 3, 1, 1, 4, 1, 0 };

/*
 * variable7 strMIB_variables: tree for strMIB
 * This variable defines function callbacks and type return information for the strMIB mib section
 */
struct variable7 strMIB_variables[] = {
	/* magic number, variable type, ro/rw, callback fn, L, oidsuffix */
#define   STRMODIDNUM           1
	{(u_char) STRMODIDNUM, ASN_UNSIGNED, RONLY, var_strModTable, 6, {1, 1, 1, 1, 1, 2}},
#define   STRMODTYPE            2
	{(u_char) STRMODTYPE, ASN_INTEGER, RONLY, var_strModTable, 6, {1, 1, 1, 1, 1, 3}},
#define   STRMODMAJOR           3
	{(u_char) STRMODMAJOR, ASN_UNSIGNED, RONLY, var_strModTable, 6, {1, 1, 1, 1, 1, 4}},
#define   STRMODROWSTATUS       4
	{(u_char) STRMODROWSTATUS, ASN_INTEGER, RWRITE, var_strModTable, 6, {1, 1, 1, 1, 1, 5}},
#define   STRMODINFOQUEUES      5
	{(u_char) STRMODINFOQUEUES, ASN_OCTET_STR, RONLY, var_strModInfoTable, 6, {1, 1, 2, 1, 1, 2}},
#define   STRMODINFOMINPSZ      6
	{(u_char) STRMODINFOMINPSZ, ASN_INTEGER, RWRITE, var_strModInfoTable, 6, {1, 1, 2, 1, 1, 3}},
#define   STRMODINFOMAXPSZ      7
	{(u_char) STRMODINFOMAXPSZ, ASN_INTEGER, RWRITE, var_strModInfoTable, 6, {1, 1, 2, 1, 1, 4}},
#define   STRMODINFOHIWAT       8
	{(u_char) STRMODINFOHIWAT, ASN_UNSIGNED, RWRITE, var_strModInfoTable, 6, {1, 1, 2, 1, 1, 5}},
#define   STRMODINFOLOWAT       9
	{(u_char) STRMODINFOLOWAT, ASN_UNSIGNED, RWRITE, var_strModInfoTable, 6, {1, 1, 2, 1, 1, 6}},
#define   STRMODINFOTRACELEVEL  10
	{(u_char) STRMODINFOTRACELEVEL, ASN_UNSIGNED, RWRITE, var_strModInfoTable, 6, {1, 1, 2, 1, 1, 7}},
#define   STRMODSTATQUEUES      11
	{(u_char) STRMODSTATQUEUES, ASN_OCTET_STR, RONLY, var_strModStatTable, 6, {1, 1, 3, 1, 1, 2}},
#define   STRMODSTATPCNT        12
	{(u_char) STRMODSTATPCNT, ASN_COUNTER, RONLY, var_strModStatTable, 6, {1, 1, 3, 1, 1, 3}},
#define   STRMODSTATSCNT        13
	{(u_char) STRMODSTATSCNT, ASN_COUNTER, RONLY, var_strModStatTable, 6, {1, 1, 3, 1, 1, 4}},
#define   STRMODSTATOCNT        14
	{(u_char) STRMODSTATOCNT, ASN_COUNTER, RONLY, var_strModStatTable, 6, {1, 1, 3, 1, 1, 5}},
#define   STRMODSTATCCNT        15
	{(u_char) STRMODSTATCCNT, ASN_COUNTER, RONLY, var_strModStatTable, 6, {1, 1, 3, 1, 1, 6}},
#define   STRMODSTATACNT        16
	{(u_char) STRMODSTATACNT, ASN_COUNTER, RONLY, var_strModStatTable, 6, {1, 1, 3, 1, 1, 7}},
#define   STRMODSTATPRIVATE     17
	{(u_char) STRMODSTATPRIVATE, ASN_OCTET_STR, RONLY, var_strModStatTable, 6, {1, 1, 3, 1, 1, 8}},
#define   STRMODSTATFLAGS       18
	{(u_char) STRMODSTATFLAGS, ASN_UNSIGNED, RONLY, var_strModStatTable, 6, {1, 1, 3, 1, 1, 9}},
#define   STRAPSHMAJOR          19
	{(u_char) STRAPSHMAJOR, ASN_UNSIGNED, RONLY, var_strApshTable, 6, {1, 1, 4, 1, 1, 1}},
#define   STRAPSHMODULES        20
	{(u_char) STRAPSHMODULES, ASN_OCTET_STR, RWRITE, var_strApshTable, 6, {1, 1, 4, 1, 1, 4}},
#define   STRAPSHROWSTATUS      21
	{(u_char) STRAPSHROWSTATUS, ASN_INTEGER, RWRITE, var_strApshTable, 6, {1, 1, 4, 1, 1, 5}},
#define   STRSTATSCURRENTALLOCS  22
	{(u_char) STRSTATSCURRENTALLOCS, ASN_GAUGE, RONLY, var_strStatsTable, 6, {1, 1, 5, 1, 1, 2}},
#define   STRSTATSHIGHWATERMARK  23
	{(u_char) STRSTATSHIGHWATERMARK, ASN_GAUGE, RONLY, var_strStatsTable, 6, {1, 1, 5, 1, 1, 3}},
#define   STRCLTIME             24
	{(u_char) STRCLTIME, ASN_INTEGER, RWRITE, var_strMIB, 4, {1, 2, 1, 1}},
#define   STRMAXAPUSH           25
	{(u_char) STRMAXAPUSH, ASN_UNSIGNED, RWRITE, var_strMIB, 4, {1, 2, 1, 2}},
#define   STRMAXMBLK            26
	{(u_char) STRMAXMBLK, ASN_UNSIGNED, RWRITE, var_strMIB, 4, {1, 2, 1, 3}},
#define   STRMAXSTRAMOD         27
	{(u_char) STRMAXSTRAMOD, ASN_UNSIGNED, RWRITE, var_strMIB, 4, {1, 2, 1, 4}},
#define   STRMAXSTRDEV          28
	{(u_char) STRMAXSTRDEV, ASN_UNSIGNED, RONLY, var_strMIB, 4, {1, 2, 1, 5}},
#define   STRMAXSTRMOD          29
	{(u_char) STRMAXSTRMOD, ASN_UNSIGNED, RONLY, var_strMIB, 4, {1, 2, 1, 6}},
#define   STRMSGPRIORITY        30
	{(u_char) STRMSGPRIORITY, ASN_INTEGER, RWRITE, var_strMIB, 4, {1, 2, 1, 7}},
#define   STRNBAND              31
	{(u_char) STRNBAND, ASN_UNSIGNED, RONLY, var_strMIB, 4, {1, 2, 1, 8}},
#define   STRNSTRMSGS           32
	{(u_char) STRNSTRMSGS, ASN_UNSIGNED, RWRITE, var_strMIB, 4, {1, 2, 1, 9}},
#define   STRNSTRPUSH           33
	{(u_char) STRNSTRPUSH, ASN_UNSIGNED, RWRITE, var_strMIB, 4, {1, 2, 1, 10}},
#define   STRHIWAT              34
	{(u_char) STRHIWAT, ASN_UNSIGNED, RWRITE, var_strMIB, 4, {1, 2, 1, 11}},
#define   STRLOWAT              35
	{(u_char) STRLOWAT, ASN_UNSIGNED, RWRITE, var_strMIB, 4, {1, 2, 1, 12}},
#define   STRMAXPSZ             36
	{(u_char) STRMAXPSZ, ASN_INTEGER, RWRITE, var_strMIB, 4, {1, 2, 1, 13}},
#define   STRMINPSZ             37
	{(u_char) STRMINPSZ, ASN_INTEGER, RWRITE, var_strMIB, 4, {1, 2, 1, 14}},
#define   STRREUSEFMODSW        38
	{(u_char) STRREUSEFMODSW, ASN_INTEGER, RWRITE, var_strMIB, 4, {1, 2, 1, 15}},
#define   STRRTIME              39
	{(u_char) STRRTIME, ASN_INTEGER, RWRITE, var_strMIB, 4, {1, 2, 1, 16}},
#define   STRSTRHOLD            40
	{(u_char) STRSTRHOLD, ASN_INTEGER, RWRITE, var_strMIB, 4, {1, 2, 1, 17}},
#define   STRSTRCTLSZ           41
	{(u_char) STRSTRCTLSZ, ASN_UNSIGNED, RWRITE, var_strMIB, 4, {1, 2, 1, 18}},
#define   STRSTRMSGSZ           42
	{(u_char) STRSTRMSGSZ, ASN_UNSIGNED, RWRITE, var_strMIB, 4, {1, 2, 1, 19}},
#define   STRSTRTHRESH          43
	{(u_char) STRSTRTHRESH, ASN_UNSIGNED, RWRITE, var_strMIB, 4, {1, 2, 1, 20}},
#define   STRLOWTHRESH          44
	{(u_char) STRLOWTHRESH, ASN_UNSIGNED, RWRITE, var_strMIB, 4, {1, 2, 1, 21}},
#define   STRMEDTHRESH          45
	{(u_char) STRMEDTHRESH, ASN_UNSIGNED, RWRITE, var_strMIB, 4, {1, 2, 1, 22}},
#define   STRIOCTIME            46
	{(u_char) STRIOCTIME, ASN_INTEGER, RWRITE, var_strMIB, 4, {1, 2, 1, 23}},
};

/* (L = length of the oidsuffix) */
struct strMIB_data *strMIBStorage = NULL;

/* global storage of our data, saved in and configured by header_complex() */
struct header_complex_index *strModTableStorage = NULL;
struct header_complex_index *strModInfoTableStorage = NULL;
struct header_complex_index *strModStatTableStorage = NULL;
struct header_complex_index *strApshTableStorage = NULL;
struct header_complex_index *strStatsTableStorage = NULL;

void (*strMIBold_signal_handler) (int) = NULL;	/* save old signal handler just in case */
void strMIB_loop_handler(int);
void strMIB_fd_handler(int, void *);

/**
 * @fn void init_strMIB(void)
 * @brief strMIB initialization routine.
 *
 * This is called when the agent starts up.  At a minimum, registration of the MIB variables
 * structure (strMIB_variables) should take place here.  By default the function also
 * registers the configuration handler and configuration store callbacks.
 *
 * Additional registrations that may be considered here are calls to regsiter_readfd(),
 * register_writefd() and register_exceptfd() for hooking into the snmpd event loop, but only when
 * used as a loadable module.  By default this function establishes a single file descriptor to
 * read, or upon which to handle exceptions.  Note that the snmpd only supports a maximum of 32
 * extneral file descriptors, so these should be used sparingly.
 *
 * When running as a loadable module, it is also necessary to hook into the snmpd event loop so that
 * the current request number can be deteremined.  This is accomplished by using a trick of the
 * external_signal_scheduled and external_signal_handler mechanism which is called on each event
 * loop when external_signal_scheduled is non-zero.  This is used to increment the sa_request value
 * on each snmpd event loop interation so that calls to MIB tree functions can determine whether
 * they belong to a fresh request or not (primarily for cacheing and possibly to clean up non-polled
 * file descriptors).
 */
void
init_strMIB(void)
{
	(void) my_fd;
	(void) zeroDotZero_oid;
	(void) snmpTrapOID_oid;
	DEBUGMSGTL(("strMIB", "init_strMIB: initializing...  "));
	/* register ourselves with the agent to handle our mib tree */
	REGISTER_MIB("strMIB", strMIB_variables, variable7, strMIB_variables_oid);
	snmp_register_callback(SNMP_CALLBACK_LIBRARY, SNMP_CALLBACK_SHUTDOWN, term_strMIB, NULL);
	/* register our config handler(s) to deal with registrations */
	snmpd_register_config_handler("strMIB", parse_strMIB, NULL, "HELP STRING");
	snmpd_register_config_handler("strModTable", parse_strModTable, NULL, "HELP STRING");
	snmpd_register_config_handler("strModInfoTable", parse_strModInfoTable, NULL, "HELP STRING");
	snmpd_register_config_handler("strModStatTable", parse_strModStatTable, NULL, "HELP STRING");
	snmpd_register_config_handler("strApshTable", parse_strApshTable, NULL, "HELP STRING");
	snmpd_register_config_handler("strStatsTable", parse_strStatsTable, NULL, "HELP STRING");

	/* we need to be called back later to store our data */
	snmp_register_callback(SNMP_CALLBACK_LIBRARY, SNMP_CALLBACK_STORE_DATA, store_strMIB, NULL);
	snmp_register_callback(SNMP_CALLBACK_LIBRARY, SNMP_CALLBACK_STORE_DATA, store_strModTable, NULL);
	snmp_register_callback(SNMP_CALLBACK_LIBRARY, SNMP_CALLBACK_STORE_DATA, store_strModInfoTable, NULL);
	snmp_register_callback(SNMP_CALLBACK_LIBRARY, SNMP_CALLBACK_STORE_DATA, store_strModStatTable, NULL);
	snmp_register_callback(SNMP_CALLBACK_LIBRARY, SNMP_CALLBACK_STORE_DATA, store_strApshTable, NULL);
	snmp_register_callback(SNMP_CALLBACK_LIBRARY, SNMP_CALLBACK_STORE_DATA, store_strStatsTable, NULL);

	/* place any other initialization junk you need here */
	if (my_readfd >= 0) {
		register_readfd(my_readfd, strMIB_fd_handler, (void *) 0);
		register_exceptfd(my_readfd, strMIB_fd_handler, (void *) 1);
	}
	strMIBold_signal_handler = external_signal_handler[SIGCHLD];
	external_signal_handler[SIGCHLD] = &strMIB_loop_handler;
	external_signal_scheduled[SIGCHLD] = 1;
	init_strExtMIB();
	DEBUGMSGTL(("strMIB", "done.\n"));
}

/**
 * @fn void deinit_strMIB(void)
 * @brief deinitialization routine.
 *
 * This is called before the agent is unloaded.  At a minimum, deregistration of the MIB variables
 * structure (strMIB_variables) should take place here.  By default, the function also
 * deregisters the the configuration file handlers for the MIB variables and table rows.
 *
 * Additional deregistrations that may be required here are calls to unregister_readfd(),
 * unregister_writefd() and unregsiter_exceptfd() for unhooking from the snmpd event loop, but only
 * when used as a loadable module.  By default if a read file descriptor exists, it is unregistered.
 */
void
deinit_strMIB(void)
{
	DEBUGMSGTL(("strMIB", "deinit_strMIB: deinitializating...  "));
	external_signal_handler[SIGCHLD] = strMIBold_signal_handler;
	if (my_readfd >= 0) {
		unregister_exceptfd(my_readfd);
		unregister_readfd(my_readfd);
		close(my_readfd);
		my_readfd = -1;
	}
	unregister_mib(strMIB_variables_oid, sizeof(strMIB_variables_oid) / sizeof(oid));
	snmpd_unregister_config_handler("strMIB");
	snmpd_unregister_config_handler("strModTable");
	snmpd_unregister_config_handler("strModInfoTable");
	snmpd_unregister_config_handler("strModStatTable");
	snmpd_unregister_config_handler("strApshTable");
	snmpd_unregister_config_handler("strStatsTable");

	/* place any other de-initialization junk you need here */
	DEBUGMSGTL(("strMIB", "done.\n"));
}

int
term_strMIB(int majorID, int minorID, void *serverarg, void *clientarg)
{
	DEBUGMSGTL(("strMIB", "term_strMIB: terminating...  "));
	deinit_strMIB();
	DEBUGMSGTL(("strMIB", "done.\n"));
	return 0;
}

/**
 * @fn struct strMIB_data *strMIB_create(void)
 * @brief create a fresh data structure representing scalars in strMIB.
 *
 * Creates a new strMIB_data structure by allocating dynamic memory for the structure and
 * initializing the default values of scalars in strMIB.
 */
struct strMIB_data *
strMIB_create(void)
{
	struct strMIB_data *StorageNew = SNMP_MALLOC_STRUCT(strMIB_data);

	DEBUGMSGTL(("strMIB", "strMIB_create: creating scalars...  "));
	if (StorageNew != NULL) {
		/* XXX: fill in default scalar values here into StorageNew */
		StorageNew->strCltime = 1500;
		StorageNew->strMaxApush = 8;
		StorageNew->strMaxMblk = 10;
		StorageNew->strMaxStramod = 8;
		StorageNew->strMaxStrdev = 16384;
		StorageNew->strMaxStrmod = 16384;
		StorageNew->strMsgPriority = 2;
		StorageNew->strNband = 256;
		StorageNew->strNstrmsgs = 4096;
		StorageNew->strNstrpush = 64;
		StorageNew->strHiwat = 5120;
		StorageNew->strLowat = 1024;
		StorageNew->strMaxpsz = 4096;
		StorageNew->strMinpsz = 0;
		StorageNew->strReuseFmodsw = 2;
		StorageNew->strRtime = 1;
		StorageNew->strStrhold = 2;
		StorageNew->strStrctlsz = 4096;
		StorageNew->strStrmsgsz = 262144;
		StorageNew->strStrthresh = 0;
		StorageNew->strLowthresh = 0;
		StorageNew->strMedthresh = 0;
		StorageNew->strIoctime = 1500;

	}
	DEBUGMSGTL(("strMIB", "done.\n"));
	return (StorageNew);
}

/**
 * @fn int strMIB_destroy(struct strMIB_data **thedata)
 * @param thedata pointer to the data structure in strMIB.
 * @brief delete a scalars structure from strMIB.
 *
 * Frees scalars that were previously removed from strMIB.  Note that the strings associated
 * with octet strings, object identifiers and bit strings still attached to the structure will also
 * be freed.  The pointer that was passed in @param thedata will be set to NULL if it is not already
 * NULL.
 */
int
strMIB_destroy(struct strMIB_data **thedata)
{
	struct strMIB_data *StorageDel;

	DEBUGMSGTL(("strMIB", "strMIB_destroy: deleting scalars...  "));
	if ((StorageDel = *thedata) != NULL) {
		SNMP_FREE(StorageDel);
		*thedata = StorageDel;
	}
	DEBUGMSGTL(("strMIB", "done.\n"));
	return SNMPERR_SUCCESS;
}

/**
 * @fn int strMIB_add(struct strMIB_data *thedata)
 * @param thedata the structure representing strMIB scalars.
 * @brief adds node to the strMIB scalar data set.
 *
 * Adds a scalar structure to the strMIB data set.  Note that this function is necessary even
 * when the scalar values are not peristent.
 */
int
strMIB_add(struct strMIB_data *thedata)
{
	DEBUGMSGTL(("strMIB", "strMIB_add: adding data...  "));
	if (thedata)
		strMIBStorage = thedata;
	DEBUGMSGTL(("strMIB", "done.\n"));
	return SNMPERR_SUCCESS;
}

/**
 * @fn void parse_strMIB(const char *token, char *line)
 * @param token token used within the configuration file.
 * @param line line from configuration file matching the token.
 * @brief parse configuration file for strMIB entries.
 *
 * This callback is called by UCD-SNMP when it prases a configuration file and finds a configuration
 * file line for the registsred token (in this case strMIB).  This routine is invoked by
 * UCD-SNMP to read the values of scalars in the MIB from the configuration file.  Note that this
 * procedure may exist regardless of the persistence of the MIB.  If there are no configured entries
 * in the configuration MIB, this function will simply not be called.
 */
void
parse_strMIB(const char *token, char *line)
{
	size_t tmpsize;
	struct strMIB_data *StorageTmp = strMIB_create();

	DEBUGMSGTL(("strMIB", "parse_strMIB: parsing config...  "));
	if (StorageTmp == NULL) {
		config_perror("malloc failure");
		return;
	}
	/* XXX: remove individual scalars that are not persistent */
	line = read_config_read_data(ASN_INTEGER, line, &StorageTmp->strCltime, &tmpsize);
	line = read_config_read_data(ASN_UNSIGNED, line, &StorageTmp->strMaxApush, &tmpsize);
	line = read_config_read_data(ASN_UNSIGNED, line, &StorageTmp->strMaxMblk, &tmpsize);
	line = read_config_read_data(ASN_UNSIGNED, line, &StorageTmp->strMaxStramod, &tmpsize);
	line = read_config_read_data(ASN_UNSIGNED, line, &StorageTmp->strMaxStrdev, &tmpsize);
	line = read_config_read_data(ASN_UNSIGNED, line, &StorageTmp->strMaxStrmod, &tmpsize);
	line = read_config_read_data(ASN_INTEGER, line, &StorageTmp->strMsgPriority, &tmpsize);
	line = read_config_read_data(ASN_UNSIGNED, line, &StorageTmp->strNband, &tmpsize);
	line = read_config_read_data(ASN_UNSIGNED, line, &StorageTmp->strNstrmsgs, &tmpsize);
	line = read_config_read_data(ASN_UNSIGNED, line, &StorageTmp->strNstrpush, &tmpsize);
	line = read_config_read_data(ASN_UNSIGNED, line, &StorageTmp->strHiwat, &tmpsize);
	line = read_config_read_data(ASN_UNSIGNED, line, &StorageTmp->strLowat, &tmpsize);
	line = read_config_read_data(ASN_INTEGER, line, &StorageTmp->strMaxpsz, &tmpsize);
	line = read_config_read_data(ASN_INTEGER, line, &StorageTmp->strMinpsz, &tmpsize);
	line = read_config_read_data(ASN_INTEGER, line, &StorageTmp->strReuseFmodsw, &tmpsize);
	line = read_config_read_data(ASN_INTEGER, line, &StorageTmp->strRtime, &tmpsize);
	line = read_config_read_data(ASN_INTEGER, line, &StorageTmp->strStrhold, &tmpsize);
	line = read_config_read_data(ASN_UNSIGNED, line, &StorageTmp->strStrctlsz, &tmpsize);
	line = read_config_read_data(ASN_UNSIGNED, line, &StorageTmp->strStrmsgsz, &tmpsize);
	line = read_config_read_data(ASN_UNSIGNED, line, &StorageTmp->strStrthresh, &tmpsize);
	line = read_config_read_data(ASN_UNSIGNED, line, &StorageTmp->strLowthresh, &tmpsize);
	line = read_config_read_data(ASN_UNSIGNED, line, &StorageTmp->strMedthresh, &tmpsize);
	line = read_config_read_data(ASN_INTEGER, line, &StorageTmp->strIoctime, &tmpsize);
	strMIB_add(StorageTmp);
	(void) tmpsize;
	DEBUGMSGTL(("strMIB", "done.\n"));
}

/*
 * store_strMIB(): stores .conf file entries needed to configure the mib.
 */
int
store_strMIB(int majorID, int minorID, void *serverarg, void *clientarg)
{
	char line[SNMP_MAXBUF];
	char *cptr;
	size_t tmpsize;
	struct strMIB_data *StorageTmp;

	DEBUGMSGTL(("strMIB", "store_strMIB: storing data...  "));
	refresh_strMIB(1);
	if ((StorageTmp = strMIBStorage) == NULL) {
		DEBUGMSGTL(("strMIB", "error.\n"));
		return SNMPERR_GENERR;
	}
	(void) tmpsize;
	/* XXX: comment entire section if no scalars are persistent */
	{
		memset(line, 0, sizeof(line));
		strcat(line, "strMIB ");
		cptr = line + strlen(line);
		/* XXX: remove individual scalars that are not persistent */
		cptr = read_config_store_data(ASN_INTEGER, cptr, &StorageTmp->strCltime, &tmpsize);
		cptr = read_config_store_data(ASN_UNSIGNED, cptr, &StorageTmp->strMaxApush, &tmpsize);
		cptr = read_config_store_data(ASN_UNSIGNED, cptr, &StorageTmp->strMaxMblk, &tmpsize);
		cptr = read_config_store_data(ASN_UNSIGNED, cptr, &StorageTmp->strMaxStramod, &tmpsize);
		cptr = read_config_store_data(ASN_UNSIGNED, cptr, &StorageTmp->strMaxStrdev, &tmpsize);
		cptr = read_config_store_data(ASN_UNSIGNED, cptr, &StorageTmp->strMaxStrmod, &tmpsize);
		cptr = read_config_store_data(ASN_INTEGER, cptr, &StorageTmp->strMsgPriority, &tmpsize);
		cptr = read_config_store_data(ASN_UNSIGNED, cptr, &StorageTmp->strNband, &tmpsize);
		cptr = read_config_store_data(ASN_UNSIGNED, cptr, &StorageTmp->strNstrmsgs, &tmpsize);
		cptr = read_config_store_data(ASN_UNSIGNED, cptr, &StorageTmp->strNstrpush, &tmpsize);
		cptr = read_config_store_data(ASN_UNSIGNED, cptr, &StorageTmp->strHiwat, &tmpsize);
		cptr = read_config_store_data(ASN_UNSIGNED, cptr, &StorageTmp->strLowat, &tmpsize);
		cptr = read_config_store_data(ASN_INTEGER, cptr, &StorageTmp->strMaxpsz, &tmpsize);
		cptr = read_config_store_data(ASN_INTEGER, cptr, &StorageTmp->strMinpsz, &tmpsize);
		cptr = read_config_store_data(ASN_INTEGER, cptr, &StorageTmp->strReuseFmodsw, &tmpsize);
		cptr = read_config_store_data(ASN_INTEGER, cptr, &StorageTmp->strRtime, &tmpsize);
		cptr = read_config_store_data(ASN_INTEGER, cptr, &StorageTmp->strStrhold, &tmpsize);
		cptr = read_config_store_data(ASN_UNSIGNED, cptr, &StorageTmp->strStrctlsz, &tmpsize);
		cptr = read_config_store_data(ASN_UNSIGNED, cptr, &StorageTmp->strStrmsgsz, &tmpsize);
		cptr = read_config_store_data(ASN_UNSIGNED, cptr, &StorageTmp->strStrthresh, &tmpsize);
		cptr = read_config_store_data(ASN_UNSIGNED, cptr, &StorageTmp->strLowthresh, &tmpsize);
		cptr = read_config_store_data(ASN_UNSIGNED, cptr, &StorageTmp->strMedthresh, &tmpsize);
		cptr = read_config_store_data(ASN_INTEGER, cptr, &StorageTmp->strIoctime, &tmpsize);
		snmpd_store_config(line);
	}
	DEBUGMSGTL(("strMIB", "done.\n"));
	return SNMPERR_SUCCESS;
}

/**
 * @fn void refresh_strMIB(int force)
 * @param force forced refresh when non-zero.
 * @brief refresh the scalar values of strMIB.
 *
 * Normally the values retrieved from the operating system are cached.  When the agent receives a
 * SIGPOLL from an open STREAMS configuration or administrative driver Stream, the STREAMS subsystem
 * indicates to the agent that the cache has been invalidated and that it should reread scalars and
 * tables from the STREAMS subsystem.  This function is used when the agent starts for the first
 * time, or after a SIGPOLL has been received (and a scalar has been requested).
 */
void
refresh_strMIB(int force)
{
	FILE *file;
	int ival;
	ulong val;

	if (strMIBStorage == NULL) {
		struct strMIB_data *StorageNew;

		if ((StorageNew = strMIB_create()) == NULL)
			return;
		strMIBStorage = StorageNew;
		strMIB_refresh = 1;
	}
	if (!force && strMIB_refresh == 0)
		return;
	DEBUGMSGTL(("strMIB", "refresh_strMIB: refreshing...  "));
	/* XXX: Update scalars as required here... */
	strMIB_refresh = 0;
	if ((file = fopen("/proc/sys/streams/cltime", "r"))) {
		val = strMIBStorage->strCltime * 10;
		if (fscanf(file, "%lu", &val)) {
			strMIBStorage->strCltime = (val + 9) / 10;
		}
		fclose(file);
	} else {
		snmp_log_perror(__FUNCTION__);
	}
	if ((file = fopen("/proc/sys/streams/max_apush", "r"))) {
		val = strMIBStorage->strMaxApush;
		if (fscanf(file, "%lu", &val)) {
			strMIBStorage->strMaxApush = val;
		}
		fclose(file);
	} else {
		snmp_log_perror(__FUNCTION__);
	}
	if ((file = fopen("/proc/sys/streams/max_mblk", "r"))) {
		val = strMIBStorage->strMaxMblk;
		if (fscanf(file, "%lu", &val)) {
			strMIBStorage->strMaxMblk = val;
		}
		fclose(file);
	} else {
		snmp_log_perror(__FUNCTION__);
	}
	if ((file = fopen("/proc/sys/streams/max_stramod", "r"))) {
		val = strMIBStorage->strMaxStramod;
		if (fscanf(file, "%lu", &val)) {
			strMIBStorage->strMaxStramod = val;
		}
		fclose(file);
	} else {
		snmp_log_perror(__FUNCTION__);
	}
	if ((file = fopen("/proc/sys/streams/max_strdev", "r"))) {
		val = strMIBStorage->strMaxStrdev;
		if (fscanf(file, "%lu", &val)) {
			strMIBStorage->strMaxStrdev = val;
		}
		fclose(file);
	} else {
		snmp_log_perror(__FUNCTION__);
	}
	if ((file = fopen("/proc/sys/streams/max_strmod", "r"))) {
		val = strMIBStorage->strMaxStrmod;
		if (fscanf(file, "%lu", &val)) {
			strMIBStorage->strMaxStrmod = val;
		}
		fclose(file);
	} else {
		snmp_log_perror(__FUNCTION__);
	}
	if ((file = fopen("/proc/sys/streams/msg_priority", "r"))) {
		ival = (strMIBStorage->strMsgPriority == TV_TRUE);
		if (fscanf(file, "%d", &ival)) {
			strMIBStorage->strMsgPriority = ival ? TV_TRUE : TV_FALSE;
		}
		fclose(file);
	} else {
		snmp_log_perror(__FUNCTION__);
	}
	if ((file = fopen("/proc/sys/streams/nband", "r"))) {
		val = strMIBStorage->strNband;
		if (fscanf(file, "%lu", &val)) {
			strMIBStorage->strNband = val;
		}
		fclose(file);
	} else {
		snmp_log_perror(__FUNCTION__);
	}
	if ((file = fopen("/proc/sys/streams/nstrmsgs", "r"))) {
		val = strMIBStorage->strNstrmsgs;
		if (fscanf(file, "%lu", &val)) {
			strMIBStorage->strNstrmsgs = val;
		}
		fclose(file);
	} else {
		snmp_log_perror(__FUNCTION__);
	}
	if ((file = fopen("/proc/sys/streams/nstrpush", "r"))) {
		val = strMIBStorage->strNstrpush;
		if (fscanf(file, "%lu", &val)) {
			strMIBStorage->strNstrpush = val;
		}
		fclose(file);
	} else {
		snmp_log_perror(__FUNCTION__);
	}
	if ((file = fopen("/proc/sys/streams/hiwat", "r"))) {
		val = strMIBStorage->strHiwat;
		if (fscanf(file, "%lu", &val)) {
			strMIBStorage->strHiwat = val;
		}
		fclose(file);
	} else {
		snmp_log_perror(__FUNCTION__);
	}
	if ((file = fopen("/proc/sys/streams/lowat", "r"))) {
		val = strMIBStorage->strLowat;
		if (fscanf(file, "%lu", &val)) {
			strMIBStorage->strLowat = val;
		}
		fclose(file);
	} else {
		snmp_log_perror(__FUNCTION__);
	}
	if ((file = fopen("/proc/sys/streams/maxpsz", "r"))) {
		val = strMIBStorage->strMaxpsz;
		if (fscanf(file, "%lu", &val)) {
			strMIBStorage->strMaxpsz = val;
		}
		fclose(file);
	} else {
		snmp_log_perror(__FUNCTION__);
	}
	if ((file = fopen("/proc/sys/streams/minpsz", "r"))) {
		val = strMIBStorage->strMinpsz;
		if (fscanf(file, "%lu", &val)) {
			strMIBStorage->strMinpsz = val;
		}
		fclose(file);
	} else {
		snmp_log_perror(__FUNCTION__);
	}
	if ((file = fopen("/proc/sys/streams/reuse_fmodsw", "r"))) {
		ival = (strMIBStorage->strReuseFmodsw == TV_TRUE);
		if (fscanf(file, "%d", &ival)) {
			strMIBStorage->strReuseFmodsw = ival ? TV_TRUE : TV_FALSE;
		}
		fclose(file);
	} else {
		snmp_log_perror(__FUNCTION__);
	}
	if ((file = fopen("/proc/sys/streams/rtime", "r"))) {
		val = strMIBStorage->strRtime * 10;
		if (fscanf(file, "%lu", &val)) {
			strMIBStorage->strRtime = (val + 9) / 10;
		}
		fclose(file);
	} else {
		snmp_log_perror(__FUNCTION__);
	}
	if ((file = fopen("/proc/sys/streams/strhold", "r"))) {
		ival = (strMIBStorage->strStrhold == TV_TRUE);
		if (fscanf(file, "%d", &ival)) {
			strMIBStorage->strStrhold = ival ? TV_TRUE : TV_FALSE;
		}
		fclose(file);
	} else {
		snmp_log_perror(__FUNCTION__);
	}
	if ((file = fopen("/proc/sys/streams/strctlsz", "r"))) {
		val = strMIBStorage->strStrctlsz;
		if (fscanf(file, "%lu", &val)) {
			strMIBStorage->strStrctlsz = val;
		}
		fclose(file);
	} else {
		snmp_log_perror(__FUNCTION__);
	}
	if ((file = fopen("/proc/sys/streams/strmsgsz", "r"))) {
		val = strMIBStorage->strStrmsgsz;
		if (fscanf(file, "%lu", &val)) {
			strMIBStorage->strStrmsgsz = val;
		}
		fclose(file);
	} else {
		snmp_log_perror(__FUNCTION__);
	}
	if ((file = fopen("/proc/sys/streams/strthresh", "r"))) {
		val = strMIBStorage->strStrthresh;
		if (fscanf(file, "%lu", &val)) {
			strMIBStorage->strStrthresh = val;
		}
		fclose(file);
	} else {
		snmp_log_perror(__FUNCTION__);
	}
	if ((file = fopen("/proc/sys/streams/strlowthresh", "r"))) {
		val = strMIBStorage->strLowthresh;
		if (fscanf(file, "%lu", &val)) {
			strMIBStorage->strLowthresh = val;
		}
		fclose(file);
	} else {
		snmp_log_perror(__FUNCTION__);
	}
	if ((file = fopen("/proc/sys/streams/strmedthresh", "r"))) {
		val = strMIBStorage->strMedthresh;
		if (fscanf(file, "%lu", &val)) {
			strMIBStorage->strMedthresh = val;
		}
		fclose(file);
	} else {
		snmp_log_perror(__FUNCTION__);
	}
	if ((file = fopen("/proc/sys/streams/ioctime", "r"))) {
		val = strMIBStorage->strIoctime * 10;
		if (fscanf(file, "%lu", &val)) {
			strMIBStorage->strIoctime = (val + 9) / 10;
		}
		fclose(file);
	} else {
		snmp_log_perror(__FUNCTION__);
	}
	DEBUGMSGTL(("strMIB", "done.\n"));
}

/**
 * @fn u_char * var_strMIB(struct variable *vp, oid *name, size_t *length, int exact, size_t *var_len, WriteMethod **write_method)
 * @param vp a pointer to the entry in the variables table for the requested variable.
 * @param name the object identifier for which to find.
 * @param length the length of the object identifier.
 * @param exact whether the name is exact.
 * @param var_len a pointer to the length of the representation of the object.
 * @param write_method a pointer to a write method for the object.
 * @brief locate variables in strMIB.
 *
 * This function returns a pointer to a memory area that is static across the request that contains
 * the UCD-SNMP representation of the scalar (so that it may be used to read from for a GET,
 * GET-NEXT or GET-BULK request).  This returned pointer may be NULL, in which case the function is
 * telling UCD-SNMP that the scalar does not exist for reading; however, if write_method is
 * overwritten with a non-NULL value, the function is telling UCD-SNMP that the scalar exists for
 * writing.  Write-only objects can be effected in this way.
 */
u_char *
var_strMIB(struct variable *vp, oid * name, size_t *length, int exact, size_t *var_len, WriteMethod ** write_method)
{
	struct strMIB_data *StorageTmp;
	u_char *rval;

	DEBUGMSGTL(("strMIB", "var_strMIB: lookup up varbind...  "));
	if (header_generic(vp, name, length, exact, var_len, write_method) == MATCH_FAILED)
		return NULL;
	/* Refresh the MIB values if required. */
	refresh_strMIB(0);
	if ((StorageTmp = strMIBStorage) == NULL) {
		DEBUGMSGTL(("strMIB", "no datastructure.\n"));
		return NULL;
	}
	*write_method = NULL;
	*var_len = 0;
	rval = NULL;
	/* This is where we do the value assignments for the mib results. */
	switch (vp->magic) {
	case (u_char) STRCLTIME:	/* ReadWrite */
		*write_method = write_strCltime;
		if (!StorageTmp)
			break;
		*var_len = sizeof(StorageTmp->strCltime);
		rval = (u_char *) &StorageTmp->strCltime;
		break;
	case (u_char) STRMAXAPUSH:	/* ReadWrite */
		*write_method = write_strMaxApush;
		if (!StorageTmp)
			break;
		*var_len = sizeof(StorageTmp->strMaxApush);
		rval = (u_char *) &StorageTmp->strMaxApush;
		break;
	case (u_char) STRMAXMBLK:	/* ReadWrite */
		*write_method = write_strMaxMblk;
		if (!StorageTmp)
			break;
		*var_len = sizeof(StorageTmp->strMaxMblk);
		rval = (u_char *) &StorageTmp->strMaxMblk;
		break;
	case (u_char) STRMAXSTRAMOD:	/* ReadWrite */
		*write_method = write_strMaxStramod;
		if (!StorageTmp)
			break;
		*var_len = sizeof(StorageTmp->strMaxStramod);
		rval = (u_char *) &StorageTmp->strMaxStramod;
		break;
	case (u_char) STRMAXSTRDEV:	/* ReadOnly */
		if (!StorageTmp)
			break;
		*var_len = sizeof(StorageTmp->strMaxStrdev);
		rval = (u_char *) &StorageTmp->strMaxStrdev;
		break;
	case (u_char) STRMAXSTRMOD:	/* ReadOnly */
		if (!StorageTmp)
			break;
		*var_len = sizeof(StorageTmp->strMaxStrmod);
		rval = (u_char *) &StorageTmp->strMaxStrmod;
		break;
	case (u_char) STRMSGPRIORITY:	/* ReadWrite */
		*write_method = write_strMsgPriority;
		if (!StorageTmp)
			break;
		*var_len = sizeof(StorageTmp->strMsgPriority);
		rval = (u_char *) &StorageTmp->strMsgPriority;
		break;
	case (u_char) STRNBAND:	/* ReadOnly */
		if (!StorageTmp)
			break;
		*var_len = sizeof(StorageTmp->strNband);
		rval = (u_char *) &StorageTmp->strNband;
		break;
	case (u_char) STRNSTRMSGS:	/* ReadWrite */
		*write_method = write_strNstrmsgs;
		if (!StorageTmp)
			break;
		*var_len = sizeof(StorageTmp->strNstrmsgs);
		rval = (u_char *) &StorageTmp->strNstrmsgs;
		break;
	case (u_char) STRNSTRPUSH:	/* ReadWrite */
		*write_method = write_strNstrpush;
		if (!StorageTmp)
			break;
		*var_len = sizeof(StorageTmp->strNstrpush);
		rval = (u_char *) &StorageTmp->strNstrpush;
		break;
	case (u_char) STRHIWAT:	/* ReadWrite */
		*write_method = write_strHiwat;
		if (!StorageTmp)
			break;
		*var_len = sizeof(StorageTmp->strHiwat);
		rval = (u_char *) &StorageTmp->strHiwat;
		break;
	case (u_char) STRLOWAT:	/* ReadWrite */
		*write_method = write_strLowat;
		if (!StorageTmp)
			break;
		*var_len = sizeof(StorageTmp->strLowat);
		rval = (u_char *) &StorageTmp->strLowat;
		break;
	case (u_char) STRMAXPSZ:	/* ReadWrite */
		*write_method = write_strMaxpsz;
		if (!StorageTmp)
			break;
		*var_len = sizeof(StorageTmp->strMaxpsz);
		rval = (u_char *) &StorageTmp->strMaxpsz;
		break;
	case (u_char) STRMINPSZ:	/* ReadWrite */
		*write_method = write_strMinpsz;
		if (!StorageTmp)
			break;
		*var_len = sizeof(StorageTmp->strMinpsz);
		rval = (u_char *) &StorageTmp->strMinpsz;
		break;
	case (u_char) STRREUSEFMODSW:	/* ReadWrite */
		*write_method = write_strReuseFmodsw;
		if (!StorageTmp)
			break;
		*var_len = sizeof(StorageTmp->strReuseFmodsw);
		rval = (u_char *) &StorageTmp->strReuseFmodsw;
		break;
	case (u_char) STRRTIME:	/* ReadWrite */
		*write_method = write_strRtime;
		if (!StorageTmp)
			break;
		*var_len = sizeof(StorageTmp->strRtime);
		rval = (u_char *) &StorageTmp->strRtime;
		break;
	case (u_char) STRSTRHOLD:	/* ReadWrite */
		*write_method = write_strStrhold;
		if (!StorageTmp)
			break;
		*var_len = sizeof(StorageTmp->strStrhold);
		rval = (u_char *) &StorageTmp->strStrhold;
		break;
	case (u_char) STRSTRCTLSZ:	/* ReadWrite */
		*write_method = write_strStrctlsz;
		if (!StorageTmp)
			break;
		*var_len = sizeof(StorageTmp->strStrctlsz);
		rval = (u_char *) &StorageTmp->strStrctlsz;
		break;
	case (u_char) STRSTRMSGSZ:	/* ReadWrite */
		*write_method = write_strStrmsgsz;
		if (!StorageTmp)
			break;
		*var_len = sizeof(StorageTmp->strStrmsgsz);
		rval = (u_char *) &StorageTmp->strStrmsgsz;
		break;
	case (u_char) STRSTRTHRESH:	/* ReadWrite */
		*write_method = write_strStrthresh;
		if (!StorageTmp)
			break;
		*var_len = sizeof(StorageTmp->strStrthresh);
		rval = (u_char *) &StorageTmp->strStrthresh;
		break;
	case (u_char) STRLOWTHRESH:	/* ReadWrite */
		*write_method = write_strLowthresh;
		if (!StorageTmp)
			break;
		*var_len = sizeof(StorageTmp->strLowthresh);
		rval = (u_char *) &StorageTmp->strLowthresh;
		break;
	case (u_char) STRMEDTHRESH:	/* ReadWrite */
		*write_method = write_strMedthresh;
		if (!StorageTmp)
			break;
		*var_len = sizeof(StorageTmp->strMedthresh);
		rval = (u_char *) &StorageTmp->strMedthresh;
		break;
	case (u_char) STRIOCTIME:	/* ReadWrite */
		*write_method = write_strIoctime;
		if (!StorageTmp)
			break;
		*var_len = sizeof(StorageTmp->strIoctime);
		rval = (u_char *) &StorageTmp->strIoctime;
		break;
	default:
		ERROR_MSG("");
	}
	if (rval)
		DEBUGMSGTL(("strMIB", "found.\n"));
	else
		DEBUGMSGTL(("strMIB", "not found.\n"));
	return (rval);
}

/**
 * @fn struct strModTable_data *strModTable_create(void)
 * @brief create a fresh data structure representing a new row in the strModTable table.
 *
 * Creates a new strModTable_data structure by allocating dynamic memory for the structure and
 * initializing the default values of columns in the table.  The row status object, if any, should
 * be set to RS_NOTREADY.
 */
struct strModTable_data *
strModTable_create(void)
{
	struct strModTable_data *StorageNew = SNMP_MALLOC_STRUCT(strModTable_data);

	DEBUGMSGTL(("strMIB", "strModTable_create: creating row...  "));
	if (StorageNew != NULL) {
		/* XXX: fill in default row values here into StorageNew */
		StorageNew->strModIdnum = 0;
		StorageNew->strModType = 0;
		StorageNew->strModMajor = 0;
		StorageNew->strModRowStatus = 0;
		StorageNew->strModRowStatus = RS_NOTREADY;
	}
	DEBUGMSGTL(("strMIB", "done.\n"));
	return (StorageNew);
}

/**
 * @fn struct strModTable_data *strModTable_duplicate(struct strModTable_data *thedata)
 * @param thedata the row structure to duplicate.
 * @brief duplicat a row structure for a table.
 *
 * Duplicates the specified row structure @param thedata and returns a pointer to the newly
 * allocated row structure on success, or NULL on failure.
 */
struct strModTable_data *
strModTable_duplicate(struct strModTable_data *thedata)
{
	struct strModTable_data *StorageNew = SNMP_MALLOC_STRUCT(strModTable_data);

	DEBUGMSGTL(("strMIB", "strModTable_duplicate: duplicating row...  "));
	if (StorageNew != NULL) {
	}
      done:
	DEBUGMSGTL(("strMIB", "done.\n"));
	return (StorageNew);
	goto destroy;
      destroy:
	strModTable_destroy(&StorageNew);
	goto done;
}

/**
 * @fn int strModTable_destroy(struct strModTable_data **thedata)
 * @param thedata pointer to the extracted or existing data structure in the table.
 * @brief delete a row structure from a table.
 *
 * Frees a table row that was previously removed from a table.  Note that the strings associated
 * with octet strings, object identifiers and bit strings still attached to the structure will also
 * be freed.  The pointer that was passed in @param thedata will be set to NULL if it is not already
 * NULL.
 */
int
strModTable_destroy(struct strModTable_data **thedata)
{
	struct strModTable_data *StorageDel;

	DEBUGMSGTL(("strMIB", "strModTable_destroy: deleting row...  "));
	if ((StorageDel = *thedata) != NULL) {
		SNMP_FREE(StorageDel->strModName);
		StorageDel->strModNameLen = 0;
		SNMP_FREE(StorageDel);
		*thedata = StorageDel;
	}
	DEBUGMSGTL(("strMIB", "done.\n"));
	return SNMPERR_SUCCESS;
}

/**
 * @fn int strModTable_add(struct strModTable_data *thedata)
 * @param thedata the structure representing the new row in the table.
 * @brief adds a row to the strModTable table data set.
 *
 * Adds a table row structure to the strModTable table.  Note that this function is necessary even
 * when the table rows are not peristent.  This function can be used within this MIB or other MIBs
 * by the agent to create rows within the table autonomously.
 */
int
strModTable_add(struct strModTable_data *thedata)
{
	struct variable_list *vars = NULL;

	DEBUGMSGTL(("strMIB", "strModTable_add: adding data...  "));
	if (thedata) {
		/* add the index variables to the varbind list, which is used by header_complex to index the data */
		/* strModName */
		snmp_varlist_add_variable(&vars, NULL, 0, ASN_OCTET_STR, (u_char *) thedata->strModName, thedata->strModNameLen);
		header_complex_add_data(&strModTableStorage, vars, thedata);
	}
	DEBUGMSGTL(("strMIB", "registered an entry.\n"));
	return SNMPERR_SUCCESS;
}

/**
 * @fn int strModTable_del(struct strModTable_data *thedata)
 * @param thedata pointer to the extracted or existing data structure in the table.
 * @brief delete a row structure from a table.
 *
 * Deletes a table row structure from the strModTable table but does not free it.  Note that this
 * function is necessary even when the table rows are not persistent.  This function can be used
 * within this MIB or another MIB by the agent to delete rows from the table autonomously.  The data
 * structure may either be already extracted from the data set, or the structure may still exist in
 * the data set.  This function will extract the row from the table if it has not already been
 * performed by the caller.
 */
int
strModTable_del(struct strModTable_data *thedata)
{
	struct strModTable_data *StorageDel;

	DEBUGMSGTL(("strMIB", "strModTable_data: deleting data...  "));
	if ((StorageDel = thedata) != NULL) {
		struct header_complex_index *hciptr;

		if ((hciptr = header_complex_find_entry(strModTableStorage, StorageDel)) != NULL)
			header_complex_extract_entry(&strModTableStorage, hciptr);
	}
	DEBUGMSGTL(("strMIB", "done.\n"));
	return SNMPERR_SUCCESS;
}

/**
 * @fn void parse_strModTable(const char *token, char *line)
 * @param token token used within the configuration file.
 * @param line line from configuration file matching the token.
 * @brief parse configuration file for strModTable entries.
 *
 * This callback is called by UCD-SNMP when it prases a configuration file and finds a configuration
 * file line for the registsred token (in this case strModTable).  This routine is invoked by UCD-SNMP
 * to read the values of each row in the table from the configuration file.  Note that this
 * procedure may exist regardless of the persistence of the table.  If there are no configured
 * entries in the configuration table, this function will simply not be called.
 */
void
parse_strModTable(const char *token, char *line)
{
	size_t tmpsize;
	struct strModTable_data *StorageTmp = strModTable_create();

	DEBUGMSGTL(("strMIB", "parse_strModTable: parsing config...  "));
	if (StorageTmp == NULL) {
		config_perror("malloc failure");
		return;
	}
	/* XXX: remove individual columns if not persistent */
	SNMP_FREE(StorageTmp->strModName);
	line = read_config_read_data(ASN_OCTET_STR, line, &StorageTmp->strModName, &StorageTmp->strModNameLen);
	if (StorageTmp->strModName == NULL) {
		config_perror("invalid specification for strModName");
		return;
	}
	line = read_config_read_data(ASN_UNSIGNED, line, &StorageTmp->strModIdnum, &tmpsize);
	line = read_config_read_data(ASN_INTEGER, line, &StorageTmp->strModType, &tmpsize);
	line = read_config_read_data(ASN_UNSIGNED, line, &StorageTmp->strModMajor, &tmpsize);
	line = read_config_read_data(ASN_INTEGER, line, &StorageTmp->strModRowStatus, &tmpsize);
	strModTable_add(StorageTmp);
	(void) tmpsize;
	DEBUGMSGTL(("strMIB", "done.\n"));
}

/*
 * store_strModTable(): store configuraiton file for strModTable
 * stores .conf file entries needed to configure the mib.
 */
int
store_strModTable(int majorID, int minorID, void *serverarg, void *clientarg)
{
	char line[SNMP_MAXBUF];
	char *cptr;
	size_t tmpsize;
	struct strModTable_data *StorageTmp;
	struct header_complex_index *hcindex;

	DEBUGMSGTL(("strMIB", "store_strModTable: storing data...  "));
	refresh_strModTable(1);
	(void) tmpsize;
	for (hcindex = strModTableStorage; hcindex != NULL; hcindex = hcindex->next) {
		StorageTmp = (struct strModTable_data *) hcindex->data;
		/* XXX: comment entire section if row not persistent */
		if (0) {
			memset(line, 0, sizeof(line));
			strcat(line, "strModTable ");
			cptr = line + strlen(line);
			/* XXX: remove individual columns if not persistent */
			cptr = read_config_store_data(ASN_OCTET_STR, cptr, &StorageTmp->strModName, &StorageTmp->strModNameLen);
			cptr = read_config_store_data(ASN_UNSIGNED, cptr, &StorageTmp->strModIdnum, &tmpsize);
			cptr = read_config_store_data(ASN_INTEGER, cptr, &StorageTmp->strModType, &tmpsize);
			cptr = read_config_store_data(ASN_UNSIGNED, cptr, &StorageTmp->strModMajor, &tmpsize);
			cptr = read_config_store_data(ASN_INTEGER, cptr, &StorageTmp->strModRowStatus, &tmpsize);
			snmpd_store_config(line);
		}
	}
	DEBUGMSGTL(("strMIB", "done.\n"));
	return SNMPERR_SUCCESS;
}

/**
 * @fn struct strModInfoTable_data *strModInfoTable_create(void)
 * @brief create a fresh data structure representing a new row in the strModInfoTable table.
 *
 * Creates a new strModInfoTable_data structure by allocating dynamic memory for the structure and
 * initializing the default values of columns in the table.  The row status object, if any, should
 * be set to RS_NOTREADY.
 */
struct strModInfoTable_data *
strModInfoTable_create(void)
{
	struct strModInfoTable_data *StorageNew = SNMP_MALLOC_STRUCT(strModInfoTable_data);

	DEBUGMSGTL(("strMIB", "strModInfoTable_create: creating row...  "));
	if (StorageNew != NULL) {
		/* XXX: fill in default row values here into StorageNew */
		if ((StorageNew->strModName = (uint8_t *) strdup("")) != NULL)
			StorageNew->strModNameLen = strlen("");
		if (memdup((u_char **) &StorageNew->strModInfoQueues, (u_char *) "\x00", 1) == SNMPERR_SUCCESS)
			StorageNew->strModInfoQueuesLen = 1;
		StorageNew->strModInfoMinpsz = 0;
		StorageNew->strModInfoMaxpsz = 0;
		StorageNew->strModInfoHiwat = 0;
		StorageNew->strModInfoLowat = 0;
		StorageNew->strModInfoTraceLevel = 0;

	}
	DEBUGMSGTL(("strMIB", "done.\n"));
	return (StorageNew);
}

/**
 * @fn struct strModInfoTable_data *strModInfoTable_duplicate(struct strModInfoTable_data *thedata)
 * @param thedata the row structure to duplicate.
 * @brief duplicat a row structure for a table.
 *
 * Duplicates the specified row structure @param thedata and returns a pointer to the newly
 * allocated row structure on success, or NULL on failure.
 */
struct strModInfoTable_data *
strModInfoTable_duplicate(struct strModInfoTable_data *thedata)
{
	struct strModInfoTable_data *StorageNew = SNMP_MALLOC_STRUCT(strModInfoTable_data);

	DEBUGMSGTL(("strMIB", "strModInfoTable_duplicate: duplicating row...  "));
	if (StorageNew != NULL) {
	}
      done:
	DEBUGMSGTL(("strMIB", "done.\n"));
	return (StorageNew);
	goto destroy;
      destroy:
	strModInfoTable_destroy(&StorageNew);
	goto done;
}

/**
 * @fn int strModInfoTable_destroy(struct strModInfoTable_data **thedata)
 * @param thedata pointer to the extracted or existing data structure in the table.
 * @brief delete a row structure from a table.
 *
 * Frees a table row that was previously removed from a table.  Note that the strings associated
 * with octet strings, object identifiers and bit strings still attached to the structure will also
 * be freed.  The pointer that was passed in @param thedata will be set to NULL if it is not already
 * NULL.
 */
int
strModInfoTable_destroy(struct strModInfoTable_data **thedata)
{
	struct strModInfoTable_data *StorageDel;

	DEBUGMSGTL(("strMIB", "strModInfoTable_destroy: deleting row...  "));
	if ((StorageDel = *thedata) != NULL) {
		SNMP_FREE(StorageDel->strModName);
		StorageDel->strModNameLen = 0;
		SNMP_FREE(StorageDel->strModInfoQueues);
		StorageDel->strModInfoQueuesLen = 0;
		SNMP_FREE(StorageDel);
		*thedata = StorageDel;
	}
	DEBUGMSGTL(("strMIB", "done.\n"));
	return SNMPERR_SUCCESS;
}

/**
 * @fn int strModInfoTable_add(struct strModInfoTable_data *thedata)
 * @param thedata the structure representing the new row in the table.
 * @brief adds a row to the strModInfoTable table data set.
 *
 * Adds a table row structure to the strModInfoTable table.  Note that this function is necessary even
 * when the table rows are not peristent.  This function can be used within this MIB or other MIBs
 * by the agent to create rows within the table autonomously.
 */
int
strModInfoTable_add(struct strModInfoTable_data *thedata)
{
	struct variable_list *vars = NULL;

	DEBUGMSGTL(("strMIB", "strModInfoTable_add: adding data...  "));
	if (thedata) {
		/* add the index variables to the varbind list, which is used by header_complex to index the data */
		/* strModName */
		snmp_varlist_add_variable(&vars, NULL, 0, ASN_OCTET_STR, (u_char *) thedata->strModName, thedata->strModNameLen);
		/* strModInfoIndex */
		snmp_varlist_add_variable(&vars, NULL, 0, ASN_UNSIGNED, (u_char *) &thedata->strModInfoIndex, sizeof(thedata->strModInfoIndex));
		header_complex_add_data(&strModInfoTableStorage, vars, thedata);
	}
	DEBUGMSGTL(("strMIB", "registered an entry.\n"));
	return SNMPERR_SUCCESS;
}

/**
 * @fn int strModInfoTable_del(struct strModInfoTable_data *thedata)
 * @param thedata pointer to the extracted or existing data structure in the table.
 * @brief delete a row structure from a table.
 *
 * Deletes a table row structure from the strModInfoTable table but does not free it.  Note that this
 * function is necessary even when the table rows are not persistent.  This function can be used
 * within this MIB or another MIB by the agent to delete rows from the table autonomously.  The data
 * structure may either be already extracted from the data set, or the structure may still exist in
 * the data set.  This function will extract the row from the table if it has not already been
 * performed by the caller.
 */
int
strModInfoTable_del(struct strModInfoTable_data *thedata)
{
	struct strModInfoTable_data *StorageDel;

	DEBUGMSGTL(("strMIB", "strModInfoTable_data: deleting data...  "));
	if ((StorageDel = thedata) != NULL) {
		struct header_complex_index *hciptr;

		if ((hciptr = header_complex_find_entry(strModInfoTableStorage, StorageDel)) != NULL)
			header_complex_extract_entry(&strModInfoTableStorage, hciptr);
	}
	DEBUGMSGTL(("strMIB", "done.\n"));
	return SNMPERR_SUCCESS;
}

/**
 * @fn void parse_strModInfoTable(const char *token, char *line)
 * @param token token used within the configuration file.
 * @param line line from configuration file matching the token.
 * @brief parse configuration file for strModInfoTable entries.
 *
 * This callback is called by UCD-SNMP when it prases a configuration file and finds a configuration
 * file line for the registsred token (in this case strModInfoTable).  This routine is invoked by UCD-SNMP
 * to read the values of each row in the table from the configuration file.  Note that this
 * procedure may exist regardless of the persistence of the table.  If there are no configured
 * entries in the configuration table, this function will simply not be called.
 */
void
parse_strModInfoTable(const char *token, char *line)
{
	size_t tmpsize;
	struct strModInfoTable_data *StorageTmp = strModInfoTable_create();

	DEBUGMSGTL(("strMIB", "parse_strModInfoTable: parsing config...  "));
	if (StorageTmp == NULL) {
		config_perror("malloc failure");
		return;
	}
	/* XXX: remove individual columns if not persistent */
	SNMP_FREE(StorageTmp->strModName);
	line = read_config_read_data(ASN_OCTET_STR, line, &StorageTmp->strModName, &StorageTmp->strModNameLen);
	if (StorageTmp->strModName == NULL) {
		config_perror("invalid specification for strModName");
		return;
	}
	line = read_config_read_data(ASN_UNSIGNED, line, &StorageTmp->strModInfoIndex, &tmpsize);
	SNMP_FREE(StorageTmp->strModInfoQueues);
	line = read_config_read_data(ASN_OCTET_STR, line, &StorageTmp->strModInfoQueues, &StorageTmp->strModInfoQueuesLen);
	if (StorageTmp->strModInfoQueues == NULL) {
		config_perror("invalid specification for strModInfoQueues");
		return;
	}
	line = read_config_read_data(ASN_INTEGER, line, &StorageTmp->strModInfoMinpsz, &tmpsize);
	line = read_config_read_data(ASN_INTEGER, line, &StorageTmp->strModInfoMaxpsz, &tmpsize);
	line = read_config_read_data(ASN_UNSIGNED, line, &StorageTmp->strModInfoHiwat, &tmpsize);
	line = read_config_read_data(ASN_UNSIGNED, line, &StorageTmp->strModInfoLowat, &tmpsize);
	line = read_config_read_data(ASN_UNSIGNED, line, &StorageTmp->strModInfoTraceLevel, &tmpsize);
	strModInfoTable_add(StorageTmp);
	(void) tmpsize;
	DEBUGMSGTL(("strMIB", "done.\n"));
}

/*
 * store_strModInfoTable(): store configuraiton file for strModInfoTable
 * stores .conf file entries needed to configure the mib.
 */
int
store_strModInfoTable(int majorID, int minorID, void *serverarg, void *clientarg)
{
	char line[SNMP_MAXBUF];
	char *cptr;
	size_t tmpsize;
	struct strModInfoTable_data *StorageTmp;
	struct header_complex_index *hcindex;

	DEBUGMSGTL(("strMIB", "store_strModInfoTable: storing data...  "));
	refresh_strModInfoTable(1);
	(void) tmpsize;
	for (hcindex = strModInfoTableStorage; hcindex != NULL; hcindex = hcindex->next) {
		StorageTmp = (struct strModInfoTable_data *) hcindex->data;
		/* XXX: comment entire section if row not persistent */
		if (0) {
			memset(line, 0, sizeof(line));
			strcat(line, "strModInfoTable ");
			cptr = line + strlen(line);
			/* XXX: remove individual columns if not persistent */
			cptr = read_config_store_data(ASN_OCTET_STR, cptr, &StorageTmp->strModName, &StorageTmp->strModNameLen);
			cptr = read_config_store_data(ASN_UNSIGNED, cptr, &StorageTmp->strModInfoIndex, &tmpsize);
			cptr = read_config_store_data(ASN_OCTET_STR, cptr, &StorageTmp->strModInfoQueues, &StorageTmp->strModInfoQueuesLen);
			cptr = read_config_store_data(ASN_INTEGER, cptr, &StorageTmp->strModInfoMinpsz, &tmpsize);
			cptr = read_config_store_data(ASN_INTEGER, cptr, &StorageTmp->strModInfoMaxpsz, &tmpsize);
			cptr = read_config_store_data(ASN_UNSIGNED, cptr, &StorageTmp->strModInfoHiwat, &tmpsize);
			cptr = read_config_store_data(ASN_UNSIGNED, cptr, &StorageTmp->strModInfoLowat, &tmpsize);
			cptr = read_config_store_data(ASN_UNSIGNED, cptr, &StorageTmp->strModInfoTraceLevel, &tmpsize);
			snmpd_store_config(line);
		}
	}
	DEBUGMSGTL(("strMIB", "done.\n"));
	return SNMPERR_SUCCESS;
}

/**
 * @fn struct strModStatTable_data *strModStatTable_create(void)
 * @brief create a fresh data structure representing a new row in the strModStatTable table.
 *
 * Creates a new strModStatTable_data structure by allocating dynamic memory for the structure and
 * initializing the default values of columns in the table.  The row status object, if any, should
 * be set to RS_NOTREADY.
 */
struct strModStatTable_data *
strModStatTable_create(void)
{
	struct strModStatTable_data *StorageNew = SNMP_MALLOC_STRUCT(strModStatTable_data);

	DEBUGMSGTL(("strMIB", "strModStatTable_create: creating row...  "));
	if (StorageNew != NULL) {
		/* XXX: fill in default row values here into StorageNew */
		if ((StorageNew->strModName = (uint8_t *) strdup("")) != NULL)
			StorageNew->strModNameLen = strlen("");
		if (memdup((u_char **) &StorageNew->strModStatQueues, (u_char *) "\x00", 1) == SNMPERR_SUCCESS)
			StorageNew->strModStatQueuesLen = 1;
		StorageNew->strModStatPcnt = 0;
		StorageNew->strModStatScnt = 0;
		StorageNew->strModStatOcnt = 0;
		StorageNew->strModStatCcnt = 0;
		StorageNew->strModStatAcnt = 0;
		if ((StorageNew->strModStatPrivate = (uint8_t *) strdup("")) != NULL)
			StorageNew->strModStatPrivateLen = strlen("");
		StorageNew->strModStatFlags = 0;

	}
	DEBUGMSGTL(("strMIB", "done.\n"));
	return (StorageNew);
}

/**
 * @fn struct strModStatTable_data *strModStatTable_duplicate(struct strModStatTable_data *thedata)
 * @param thedata the row structure to duplicate.
 * @brief duplicat a row structure for a table.
 *
 * Duplicates the specified row structure @param thedata and returns a pointer to the newly
 * allocated row structure on success, or NULL on failure.
 */
struct strModStatTable_data *
strModStatTable_duplicate(struct strModStatTable_data *thedata)
{
	struct strModStatTable_data *StorageNew = SNMP_MALLOC_STRUCT(strModStatTable_data);

	DEBUGMSGTL(("strMIB", "strModStatTable_duplicate: duplicating row...  "));
	if (StorageNew != NULL) {
	}
      done:
	DEBUGMSGTL(("strMIB", "done.\n"));
	return (StorageNew);
	goto destroy;
      destroy:
	strModStatTable_destroy(&StorageNew);
	goto done;
}

/**
 * @fn int strModStatTable_destroy(struct strModStatTable_data **thedata)
 * @param thedata pointer to the extracted or existing data structure in the table.
 * @brief delete a row structure from a table.
 *
 * Frees a table row that was previously removed from a table.  Note that the strings associated
 * with octet strings, object identifiers and bit strings still attached to the structure will also
 * be freed.  The pointer that was passed in @param thedata will be set to NULL if it is not already
 * NULL.
 */
int
strModStatTable_destroy(struct strModStatTable_data **thedata)
{
	struct strModStatTable_data *StorageDel;

	DEBUGMSGTL(("strMIB", "strModStatTable_destroy: deleting row...  "));
	if ((StorageDel = *thedata) != NULL) {
		SNMP_FREE(StorageDel->strModName);
		StorageDel->strModNameLen = 0;
		SNMP_FREE(StorageDel->strModStatQueues);
		StorageDel->strModStatQueuesLen = 0;
		SNMP_FREE(StorageDel->strModStatPrivate);
		StorageDel->strModStatPrivateLen = 0;
		SNMP_FREE(StorageDel);
		*thedata = StorageDel;
	}
	DEBUGMSGTL(("strMIB", "done.\n"));
	return SNMPERR_SUCCESS;
}

/**
 * @fn int strModStatTable_add(struct strModStatTable_data *thedata)
 * @param thedata the structure representing the new row in the table.
 * @brief adds a row to the strModStatTable table data set.
 *
 * Adds a table row structure to the strModStatTable table.  Note that this function is necessary even
 * when the table rows are not peristent.  This function can be used within this MIB or other MIBs
 * by the agent to create rows within the table autonomously.
 */
int
strModStatTable_add(struct strModStatTable_data *thedata)
{
	struct variable_list *vars = NULL;

	DEBUGMSGTL(("strMIB", "strModStatTable_add: adding data...  "));
	if (thedata) {
		/* add the index variables to the varbind list, which is used by header_complex to index the data */
		/* strModName */
		snmp_varlist_add_variable(&vars, NULL, 0, ASN_OCTET_STR, (u_char *) thedata->strModName, thedata->strModNameLen);
		/* strModStatIndex */
		snmp_varlist_add_variable(&vars, NULL, 0, ASN_UNSIGNED, (u_char *) &thedata->strModStatIndex, sizeof(thedata->strModStatIndex));
		header_complex_add_data(&strModStatTableStorage, vars, thedata);
	}
	DEBUGMSGTL(("strMIB", "registered an entry.\n"));
	return SNMPERR_SUCCESS;
}

/**
 * @fn int strModStatTable_del(struct strModStatTable_data *thedata)
 * @param thedata pointer to the extracted or existing data structure in the table.
 * @brief delete a row structure from a table.
 *
 * Deletes a table row structure from the strModStatTable table but does not free it.  Note that this
 * function is necessary even when the table rows are not persistent.  This function can be used
 * within this MIB or another MIB by the agent to delete rows from the table autonomously.  The data
 * structure may either be already extracted from the data set, or the structure may still exist in
 * the data set.  This function will extract the row from the table if it has not already been
 * performed by the caller.
 */
int
strModStatTable_del(struct strModStatTable_data *thedata)
{
	struct strModStatTable_data *StorageDel;

	DEBUGMSGTL(("strMIB", "strModStatTable_data: deleting data...  "));
	if ((StorageDel = thedata) != NULL) {
		struct header_complex_index *hciptr;

		if ((hciptr = header_complex_find_entry(strModStatTableStorage, StorageDel)) != NULL)
			header_complex_extract_entry(&strModStatTableStorage, hciptr);
	}
	DEBUGMSGTL(("strMIB", "done.\n"));
	return SNMPERR_SUCCESS;
}

/**
 * @fn void parse_strModStatTable(const char *token, char *line)
 * @param token token used within the configuration file.
 * @param line line from configuration file matching the token.
 * @brief parse configuration file for strModStatTable entries.
 *
 * This callback is called by UCD-SNMP when it prases a configuration file and finds a configuration
 * file line for the registsred token (in this case strModStatTable).  This routine is invoked by UCD-SNMP
 * to read the values of each row in the table from the configuration file.  Note that this
 * procedure may exist regardless of the persistence of the table.  If there are no configured
 * entries in the configuration table, this function will simply not be called.
 */
void
parse_strModStatTable(const char *token, char *line)
{
	size_t tmpsize;
	struct strModStatTable_data *StorageTmp = strModStatTable_create();

	DEBUGMSGTL(("strMIB", "parse_strModStatTable: parsing config...  "));
	if (StorageTmp == NULL) {
		config_perror("malloc failure");
		return;
	}
	/* XXX: remove individual columns if not persistent */
	SNMP_FREE(StorageTmp->strModName);
	line = read_config_read_data(ASN_OCTET_STR, line, &StorageTmp->strModName, &StorageTmp->strModNameLen);
	if (StorageTmp->strModName == NULL) {
		config_perror("invalid specification for strModName");
		return;
	}
	line = read_config_read_data(ASN_UNSIGNED, line, &StorageTmp->strModStatIndex, &tmpsize);
	SNMP_FREE(StorageTmp->strModStatQueues);
	line = read_config_read_data(ASN_OCTET_STR, line, &StorageTmp->strModStatQueues, &StorageTmp->strModStatQueuesLen);
	if (StorageTmp->strModStatQueues == NULL) {
		config_perror("invalid specification for strModStatQueues");
		return;
	}
	line = read_config_read_data(ASN_COUNTER, line, &StorageTmp->strModStatPcnt, &tmpsize);
	line = read_config_read_data(ASN_COUNTER, line, &StorageTmp->strModStatScnt, &tmpsize);
	line = read_config_read_data(ASN_COUNTER, line, &StorageTmp->strModStatOcnt, &tmpsize);
	line = read_config_read_data(ASN_COUNTER, line, &StorageTmp->strModStatCcnt, &tmpsize);
	line = read_config_read_data(ASN_COUNTER, line, &StorageTmp->strModStatAcnt, &tmpsize);
	SNMP_FREE(StorageTmp->strModStatPrivate);
	line = read_config_read_data(ASN_OCTET_STR, line, &StorageTmp->strModStatPrivate, &StorageTmp->strModStatPrivateLen);
	if (StorageTmp->strModStatPrivate == NULL) {
		config_perror("invalid specification for strModStatPrivate");
		return;
	}
	line = read_config_read_data(ASN_UNSIGNED, line, &StorageTmp->strModStatFlags, &tmpsize);
	strModStatTable_add(StorageTmp);
	(void) tmpsize;
	DEBUGMSGTL(("strMIB", "done.\n"));
}

/*
 * store_strModStatTable(): store configuraiton file for strModStatTable
 * stores .conf file entries needed to configure the mib.
 */
int
store_strModStatTable(int majorID, int minorID, void *serverarg, void *clientarg)
{
	char line[SNMP_MAXBUF];
	char *cptr;
	size_t tmpsize;
	struct strModStatTable_data *StorageTmp;
	struct header_complex_index *hcindex;

	DEBUGMSGTL(("strMIB", "store_strModStatTable: storing data...  "));
	refresh_strModStatTable(1);
	(void) tmpsize;
	for (hcindex = strModStatTableStorage; hcindex != NULL; hcindex = hcindex->next) {
		StorageTmp = (struct strModStatTable_data *) hcindex->data;
		/* XXX: comment entire section if row not persistent */
		if (0) {
			memset(line, 0, sizeof(line));
			strcat(line, "strModStatTable ");
			cptr = line + strlen(line);
			/* XXX: remove individual columns if not persistent */
			cptr = read_config_store_data(ASN_OCTET_STR, cptr, &StorageTmp->strModName, &StorageTmp->strModNameLen);
			cptr = read_config_store_data(ASN_UNSIGNED, cptr, &StorageTmp->strModStatIndex, &tmpsize);
			cptr = read_config_store_data(ASN_OCTET_STR, cptr, &StorageTmp->strModStatQueues, &StorageTmp->strModStatQueuesLen);
			cptr = read_config_store_data(ASN_COUNTER, cptr, &StorageTmp->strModStatPcnt, &tmpsize);
			cptr = read_config_store_data(ASN_COUNTER, cptr, &StorageTmp->strModStatScnt, &tmpsize);
			cptr = read_config_store_data(ASN_COUNTER, cptr, &StorageTmp->strModStatOcnt, &tmpsize);
			cptr = read_config_store_data(ASN_COUNTER, cptr, &StorageTmp->strModStatCcnt, &tmpsize);
			cptr = read_config_store_data(ASN_COUNTER, cptr, &StorageTmp->strModStatAcnt, &tmpsize);
			cptr = read_config_store_data(ASN_OCTET_STR, cptr, &StorageTmp->strModStatPrivate, &StorageTmp->strModStatPrivateLen);
			cptr = read_config_store_data(ASN_UNSIGNED, cptr, &StorageTmp->strModStatFlags, &tmpsize);
			snmpd_store_config(line);
		}
	}
	DEBUGMSGTL(("strMIB", "done.\n"));
	return SNMPERR_SUCCESS;
}

/**
 * @fn struct strApshTable_data *strApshTable_create(void)
 * @brief create a fresh data structure representing a new row in the strApshTable table.
 *
 * Creates a new strApshTable_data structure by allocating dynamic memory for the structure and
 * initializing the default values of columns in the table.  The row status object, if any, should
 * be set to RS_NOTREADY.
 */
struct strApshTable_data *
strApshTable_create(void)
{
	struct strApshTable_data *StorageNew = SNMP_MALLOC_STRUCT(strApshTable_data);

	DEBUGMSGTL(("strMIB", "strApshTable_create: creating row...  "));
	if (StorageNew != NULL) {
		/* XXX: fill in default row values here into StorageNew */
		if ((StorageNew->strModName = (uint8_t *) strdup("")) != NULL)
			StorageNew->strModNameLen = strlen("");
		StorageNew->strApshMajor = 0;
		if ((StorageNew->strApshModules = (uint8_t *) strdup("")) != NULL)
			StorageNew->strApshModulesLen = strlen("");
		StorageNew->strApshRowStatus = 0;
		StorageNew->strApshRowStatus = RS_NOTREADY;
	}
	DEBUGMSGTL(("strMIB", "done.\n"));
	return (StorageNew);
}

/**
 * @fn struct strApshTable_data *strApshTable_duplicate(struct strApshTable_data *thedata)
 * @param thedata the row structure to duplicate.
 * @brief duplicat a row structure for a table.
 *
 * Duplicates the specified row structure @param thedata and returns a pointer to the newly
 * allocated row structure on success, or NULL on failure.
 */
struct strApshTable_data *
strApshTable_duplicate(struct strApshTable_data *thedata)
{
	struct strApshTable_data *StorageNew = SNMP_MALLOC_STRUCT(strApshTable_data);

	DEBUGMSGTL(("strMIB", "strApshTable_duplicate: duplicating row...  "));
	if (StorageNew != NULL) {
	}
      done:
	DEBUGMSGTL(("strMIB", "done.\n"));
	return (StorageNew);
	goto destroy;
      destroy:
	strApshTable_destroy(&StorageNew);
	goto done;
}

/**
 * @fn int strApshTable_destroy(struct strApshTable_data **thedata)
 * @param thedata pointer to the extracted or existing data structure in the table.
 * @brief delete a row structure from a table.
 *
 * Frees a table row that was previously removed from a table.  Note that the strings associated
 * with octet strings, object identifiers and bit strings still attached to the structure will also
 * be freed.  The pointer that was passed in @param thedata will be set to NULL if it is not already
 * NULL.
 */
int
strApshTable_destroy(struct strApshTable_data **thedata)
{
	struct strApshTable_data *StorageDel;

	DEBUGMSGTL(("strMIB", "strApshTable_destroy: deleting row...  "));
	if ((StorageDel = *thedata) != NULL) {
		SNMP_FREE(StorageDel->strModName);
		StorageDel->strModNameLen = 0;
		SNMP_FREE(StorageDel->strApshModules);
		StorageDel->strApshModulesLen = 0;
		SNMP_FREE(StorageDel);
		*thedata = StorageDel;
	}
	DEBUGMSGTL(("strMIB", "done.\n"));
	return SNMPERR_SUCCESS;
}

/**
 * @fn int strApshTable_add(struct strApshTable_data *thedata)
 * @param thedata the structure representing the new row in the table.
 * @brief adds a row to the strApshTable table data set.
 *
 * Adds a table row structure to the strApshTable table.  Note that this function is necessary even
 * when the table rows are not peristent.  This function can be used within this MIB or other MIBs
 * by the agent to create rows within the table autonomously.
 */
int
strApshTable_add(struct strApshTable_data *thedata)
{
	struct variable_list *vars = NULL;

	DEBUGMSGTL(("strMIB", "strApshTable_add: adding data...  "));
	if (thedata) {
		/* add the index variables to the varbind list, which is used by header_complex to index the data */
		/* strModName */
		snmp_varlist_add_variable(&vars, NULL, 0, ASN_OCTET_STR, (u_char *) thedata->strModName, thedata->strModNameLen);
		/* strApshMinor */
		snmp_varlist_add_variable(&vars, NULL, 0, ASN_UNSIGNED, (u_char *) &thedata->strApshMinor, sizeof(thedata->strApshMinor));
		/* strApshLastMinor */
		snmp_varlist_add_variable(&vars, NULL, 0, ASN_UNSIGNED, (u_char *) &thedata->strApshLastMinor, sizeof(thedata->strApshLastMinor));
		header_complex_add_data(&strApshTableStorage, vars, thedata);
	}
	DEBUGMSGTL(("strMIB", "registered an entry.\n"));
	return SNMPERR_SUCCESS;
}

/**
 * @fn int strApshTable_del(struct strApshTable_data *thedata)
 * @param thedata pointer to the extracted or existing data structure in the table.
 * @brief delete a row structure from a table.
 *
 * Deletes a table row structure from the strApshTable table but does not free it.  Note that this
 * function is necessary even when the table rows are not persistent.  This function can be used
 * within this MIB or another MIB by the agent to delete rows from the table autonomously.  The data
 * structure may either be already extracted from the data set, or the structure may still exist in
 * the data set.  This function will extract the row from the table if it has not already been
 * performed by the caller.
 */
int
strApshTable_del(struct strApshTable_data *thedata)
{
	struct strApshTable_data *StorageDel;

	DEBUGMSGTL(("strMIB", "strApshTable_data: deleting data...  "));
	if ((StorageDel = thedata) != NULL) {
		struct header_complex_index *hciptr;

		if ((hciptr = header_complex_find_entry(strApshTableStorage, StorageDel)) != NULL)
			header_complex_extract_entry(&strApshTableStorage, hciptr);
	}
	DEBUGMSGTL(("strMIB", "done.\n"));
	return SNMPERR_SUCCESS;
}

/**
 * @fn void parse_strApshTable(const char *token, char *line)
 * @param token token used within the configuration file.
 * @param line line from configuration file matching the token.
 * @brief parse configuration file for strApshTable entries.
 *
 * This callback is called by UCD-SNMP when it prases a configuration file and finds a configuration
 * file line for the registsred token (in this case strApshTable).  This routine is invoked by UCD-SNMP
 * to read the values of each row in the table from the configuration file.  Note that this
 * procedure may exist regardless of the persistence of the table.  If there are no configured
 * entries in the configuration table, this function will simply not be called.
 */
void
parse_strApshTable(const char *token, char *line)
{
	size_t tmpsize;
	struct strApshTable_data *StorageTmp = strApshTable_create();

	DEBUGMSGTL(("strMIB", "parse_strApshTable: parsing config...  "));
	if (StorageTmp == NULL) {
		config_perror("malloc failure");
		return;
	}
	/* XXX: remove individual columns if not persistent */
	SNMP_FREE(StorageTmp->strModName);
	line = read_config_read_data(ASN_OCTET_STR, line, &StorageTmp->strModName, &StorageTmp->strModNameLen);
	if (StorageTmp->strModName == NULL) {
		config_perror("invalid specification for strModName");
		return;
	}
	line = read_config_read_data(ASN_UNSIGNED, line, &StorageTmp->strApshMajor, &tmpsize);
	line = read_config_read_data(ASN_UNSIGNED, line, &StorageTmp->strApshMinor, &tmpsize);
	line = read_config_read_data(ASN_UNSIGNED, line, &StorageTmp->strApshLastMinor, &tmpsize);
	SNMP_FREE(StorageTmp->strApshModules);
	line = read_config_read_data(ASN_OCTET_STR, line, &StorageTmp->strApshModules, &StorageTmp->strApshModulesLen);
	if (StorageTmp->strApshModules == NULL) {
		config_perror("invalid specification for strApshModules");
		return;
	}
	line = read_config_read_data(ASN_INTEGER, line, &StorageTmp->strApshRowStatus, &tmpsize);
	strApshTable_add(StorageTmp);
	(void) tmpsize;
	DEBUGMSGTL(("strMIB", "done.\n"));
}

/*
 * store_strApshTable(): store configuraiton file for strApshTable
 * stores .conf file entries needed to configure the mib.
 */
int
store_strApshTable(int majorID, int minorID, void *serverarg, void *clientarg)
{
	char line[SNMP_MAXBUF];
	char *cptr;
	size_t tmpsize;
	struct strApshTable_data *StorageTmp;
	struct header_complex_index *hcindex;

	DEBUGMSGTL(("strMIB", "store_strApshTable: storing data...  "));
	refresh_strApshTable(1);
	(void) tmpsize;
	for (hcindex = strApshTableStorage; hcindex != NULL; hcindex = hcindex->next) {
		StorageTmp = (struct strApshTable_data *) hcindex->data;
		/* XXX: comment entire section if row not persistent */
		{
			memset(line, 0, sizeof(line));
			strcat(line, "strApshTable ");
			cptr = line + strlen(line);
			/* XXX: remove individual columns if not persistent */
			cptr = read_config_store_data(ASN_OCTET_STR, cptr, &StorageTmp->strModName, &StorageTmp->strModNameLen);
			cptr = read_config_store_data(ASN_UNSIGNED, cptr, &StorageTmp->strApshMajor, &tmpsize);
			cptr = read_config_store_data(ASN_UNSIGNED, cptr, &StorageTmp->strApshMinor, &tmpsize);
			cptr = read_config_store_data(ASN_UNSIGNED, cptr, &StorageTmp->strApshLastMinor, &tmpsize);
			cptr = read_config_store_data(ASN_OCTET_STR, cptr, &StorageTmp->strApshModules, &StorageTmp->strApshModulesLen);
			cptr = read_config_store_data(ASN_INTEGER, cptr, &StorageTmp->strApshRowStatus, &tmpsize);
			snmpd_store_config(line);
		}
	}
	DEBUGMSGTL(("strMIB", "done.\n"));
	return SNMPERR_SUCCESS;
}

/**
 * @fn struct strStatsTable_data *strStatsTable_create(void)
 * @brief create a fresh data structure representing a new row in the strStatsTable table.
 *
 * Creates a new strStatsTable_data structure by allocating dynamic memory for the structure and
 * initializing the default values of columns in the table.  The row status object, if any, should
 * be set to RS_NOTREADY.
 */
struct strStatsTable_data *
strStatsTable_create(void)
{
	struct strStatsTable_data *StorageNew = SNMP_MALLOC_STRUCT(strStatsTable_data);

	DEBUGMSGTL(("strMIB", "strStatsTable_create: creating row...  "));
	if (StorageNew != NULL) {
		/* XXX: fill in default row values here into StorageNew */
		StorageNew->strStatsCurrentAllocs = 0;
		StorageNew->strStatsHighWaterMark = 0;

	}
	DEBUGMSGTL(("strMIB", "done.\n"));
	return (StorageNew);
}

/**
 * @fn struct strStatsTable_data *strStatsTable_duplicate(struct strStatsTable_data *thedata)
 * @param thedata the row structure to duplicate.
 * @brief duplicat a row structure for a table.
 *
 * Duplicates the specified row structure @param thedata and returns a pointer to the newly
 * allocated row structure on success, or NULL on failure.
 */
struct strStatsTable_data *
strStatsTable_duplicate(struct strStatsTable_data *thedata)
{
	struct strStatsTable_data *StorageNew = SNMP_MALLOC_STRUCT(strStatsTable_data);

	DEBUGMSGTL(("strMIB", "strStatsTable_duplicate: duplicating row...  "));
	if (StorageNew != NULL) {
	}
      done:
	DEBUGMSGTL(("strMIB", "done.\n"));
	return (StorageNew);
	goto destroy;
      destroy:
	strStatsTable_destroy(&StorageNew);
	goto done;
}

/**
 * @fn int strStatsTable_destroy(struct strStatsTable_data **thedata)
 * @param thedata pointer to the extracted or existing data structure in the table.
 * @brief delete a row structure from a table.
 *
 * Frees a table row that was previously removed from a table.  Note that the strings associated
 * with octet strings, object identifiers and bit strings still attached to the structure will also
 * be freed.  The pointer that was passed in @param thedata will be set to NULL if it is not already
 * NULL.
 */
int
strStatsTable_destroy(struct strStatsTable_data **thedata)
{
	struct strStatsTable_data *StorageDel;

	DEBUGMSGTL(("strMIB", "strStatsTable_destroy: deleting row...  "));
	if ((StorageDel = *thedata) != NULL) {
		SNMP_FREE(StorageDel);
		*thedata = StorageDel;
	}
	DEBUGMSGTL(("strMIB", "done.\n"));
	return SNMPERR_SUCCESS;
}

/**
 * @fn int strStatsTable_add(struct strStatsTable_data *thedata)
 * @param thedata the structure representing the new row in the table.
 * @brief adds a row to the strStatsTable table data set.
 *
 * Adds a table row structure to the strStatsTable table.  Note that this function is necessary even
 * when the table rows are not peristent.  This function can be used within this MIB or other MIBs
 * by the agent to create rows within the table autonomously.
 */
int
strStatsTable_add(struct strStatsTable_data *thedata)
{
	struct variable_list *vars = NULL;

	DEBUGMSGTL(("strMIB", "strStatsTable_add: adding data...  "));
	if (thedata) {
		/* add the index variables to the varbind list, which is used by header_complex to index the data */
		/* strStatsStructure */
		snmp_varlist_add_variable(&vars, NULL, 0, ASN_INTEGER, (u_char *) &thedata->strStatsStructure, sizeof(thedata->strStatsStructure));
		header_complex_add_data(&strStatsTableStorage, vars, thedata);
	}
	DEBUGMSGTL(("strMIB", "registered an entry.\n"));
	return SNMPERR_SUCCESS;
}

/**
 * @fn int strStatsTable_del(struct strStatsTable_data *thedata)
 * @param thedata pointer to the extracted or existing data structure in the table.
 * @brief delete a row structure from a table.
 *
 * Deletes a table row structure from the strStatsTable table but does not free it.  Note that this
 * function is necessary even when the table rows are not persistent.  This function can be used
 * within this MIB or another MIB by the agent to delete rows from the table autonomously.  The data
 * structure may either be already extracted from the data set, or the structure may still exist in
 * the data set.  This function will extract the row from the table if it has not already been
 * performed by the caller.
 */
int
strStatsTable_del(struct strStatsTable_data *thedata)
{
	struct strStatsTable_data *StorageDel;

	DEBUGMSGTL(("strMIB", "strStatsTable_data: deleting data...  "));
	if ((StorageDel = thedata) != NULL) {
		struct header_complex_index *hciptr;

		if ((hciptr = header_complex_find_entry(strStatsTableStorage, StorageDel)) != NULL)
			header_complex_extract_entry(&strStatsTableStorage, hciptr);
	}
	DEBUGMSGTL(("strMIB", "done.\n"));
	return SNMPERR_SUCCESS;
}

/**
 * @fn void parse_strStatsTable(const char *token, char *line)
 * @param token token used within the configuration file.
 * @param line line from configuration file matching the token.
 * @brief parse configuration file for strStatsTable entries.
 *
 * This callback is called by UCD-SNMP when it prases a configuration file and finds a configuration
 * file line for the registsred token (in this case strStatsTable).  This routine is invoked by UCD-SNMP
 * to read the values of each row in the table from the configuration file.  Note that this
 * procedure may exist regardless of the persistence of the table.  If there are no configured
 * entries in the configuration table, this function will simply not be called.
 */
void
parse_strStatsTable(const char *token, char *line)
{
	size_t tmpsize;
	struct strStatsTable_data *StorageTmp = strStatsTable_create();

	DEBUGMSGTL(("strMIB", "parse_strStatsTable: parsing config...  "));
	if (StorageTmp == NULL) {
		config_perror("malloc failure");
		return;
	}
	/* XXX: remove individual columns if not persistent */
	line = read_config_read_data(ASN_INTEGER, line, &StorageTmp->strStatsStructure, &tmpsize);
	line = read_config_read_data(ASN_GAUGE, line, &StorageTmp->strStatsCurrentAllocs, &tmpsize);
	line = read_config_read_data(ASN_GAUGE, line, &StorageTmp->strStatsHighWaterMark, &tmpsize);
	strStatsTable_add(StorageTmp);
	(void) tmpsize;
	DEBUGMSGTL(("strMIB", "done.\n"));
}

/*
 * store_strStatsTable(): store configuraiton file for strStatsTable
 * stores .conf file entries needed to configure the mib.
 */
int
store_strStatsTable(int majorID, int minorID, void *serverarg, void *clientarg)
{
	char line[SNMP_MAXBUF];
	char *cptr;
	size_t tmpsize;
	struct strStatsTable_data *StorageTmp;
	struct header_complex_index *hcindex;

	DEBUGMSGTL(("strMIB", "store_strStatsTable: storing data...  "));
	refresh_strStatsTable(1);
	(void) tmpsize;
	for (hcindex = strStatsTableStorage; hcindex != NULL; hcindex = hcindex->next) {
		StorageTmp = (struct strStatsTable_data *) hcindex->data;
		/* XXX: comment entire section if row not persistent */
		{
			memset(line, 0, sizeof(line));
			strcat(line, "strStatsTable ");
			cptr = line + strlen(line);
			/* XXX: remove individual columns if not persistent */
			cptr = read_config_store_data(ASN_INTEGER, cptr, &StorageTmp->strStatsStructure, &tmpsize);
			cptr = read_config_store_data(ASN_GAUGE, cptr, &StorageTmp->strStatsCurrentAllocs, &tmpsize);
			cptr = read_config_store_data(ASN_GAUGE, cptr, &StorageTmp->strStatsHighWaterMark, &tmpsize);
			snmpd_store_config(line);
		}
	}
	DEBUGMSGTL(("strMIB", "done.\n"));
	return SNMPERR_SUCCESS;
}

/**
 * @fn void refresh_strModTable_row(struct strModTable_data *StorageTmp, int force)
 * @param StorageTmp the data row to refresh.
 * @param force force refresh if non-zero.
 * @brief refresh the contents of the strModTable row.
 *
 * Normally the values retrieved from the operating system are cached.  However, if a row contains
 * temporal values, such as statistics counters, gauges, timestamps, or other transient columns, it
 * may be necessary to refresh the row on some other basis, but normally only once per request.
 */
struct strModTable_data *
refresh_strModTable_row(struct strModTable_data *StorageTmp, int force)
{
	if (!StorageTmp || (!force && StorageTmp->strModTable_request == sa_request))
		return (StorageTmp);
	/* XXX: update row; delete it and return NULL if the row has disappeared */
	StorageTmp->strModTable_request = sa_request;
	return (StorageTmp);
}

/**
 * @fn void refresh_strModTable(int force)
 * @param force force refresh if non-zero.
 * @brief refresh the scalar values of the strModTable.
 *
 * Normally the values retrieved from the operating system are cached.  When the agent receives a
 * SIGPOLL from an open STREAMS configuration or administrative driver Stream, the STREAMS subsystem
 * indicates to the agent that the cache has been invalidated and that it should reread scalars and
 * tables from the STREAMS subsystem.  This function is used when the agent starts for the first
 * time, or after a SIGPOLL has been received (and a row or column has been requested).
 */
void
refresh_strModTable(int force)
{
	if (!force && strModTable_refresh == 0)
		return;
	/* XXX: Here, update the table as required... */
	strModTable_refresh = 0;
	strModInfoTable_refresh = 0;
	strModStatTable_refresh = 0;
	{
		int fd, count, i, j, modid, len;
		struct sc_list *list = NULL;
		struct header_complex_index *hciptr, *hciptr_next;

		if ((fd = open("/dev/streams/clone/sad", O_RDWR)) < 0)
			goto error_out;
		if (ioctl(fd, I_PUSH, "sc") < 0)
			goto error_out;
		if ((count = ioctl(fd, SC_IOC_LIST, NULL)) < 0)
			goto error_out;
		if ((list = malloc(sizeof(struct sc_list) + count * sizeof(struct sc_mlist))) == NULL)
			goto error_out;
		list->sc_nmods = count;
		list->sc_mlist = (struct sc_mlist *) (list + 1);
		if (ioctl(fd, SC_IOC_LIST, list) < 0)
			goto error_out;
		close(fd);
		fd = -1;
		for (i = 0; i < count; i++) {
			char *tmp;

			if (list->sc_mlist[i].major == -1)
				continue;
			if (list->sc_mlist[i].mi[0].index == 0)
				continue;
			modid = list->sc_mlist[i].mi[0].mi_idnum;
			len = strnlen(list->sc_mlist[i].name, FMNAMESZ);
			/* update the strModTable */
			for (hciptr = strModTableStorage; hciptr; hciptr = hciptr->next) {
				struct strModTable_data *StorageTmp = hciptr->data;

				if (StorageTmp->strModNameLen == len && strncmp((char *)StorageTmp->strModName, list->sc_mlist[i].name, len) == 0)
					break;
			}
			if (hciptr) {
				struct strModTable_data *StorageTmp = hciptr->data;

				/* already have an entry for this modid, simply refresh the contents */
				StorageTmp->strModTable_request = sa_request;	/* mark as updated */
				StorageTmp->strModTable_refs++;	/* mark as used */
				StorageTmp->strModRowStatus = RS_ACTIVE;
				if ((StorageTmp->strModMajor = list->sc_mlist[i].major) == 0)
					StorageTmp->strModType = STRMODTYPE_MODULE;
				else
					StorageTmp->strModType = STRMODTYPE_DRIVER;
				if ((StorageTmp->strModNameLen != len) || (strncmp((char *) StorageTmp->strModName, list->sc_mlist[i].name, len))) {
					/* different name value */
					if (len <= StorageTmp->strModNameLen) {
						/* simply overwrite */
						memcpy((char *) StorageTmp->strModName, list->sc_mlist[i].name, len);
						StorageTmp->strModNameLen = len;
					} else if ((tmp = calloc(len + 1, 1)) != NULL) {
						memcpy(tmp, list->sc_mlist[i].name, len);
						tmp[len] = '\0';
						SNMP_FREE(StorageTmp->strModName);
						StorageTmp->strModName = (u_char *) tmp;
						StorageTmp->strModNameLen = len;
					} else
						continue;
				}
			} else {
				struct strModTable_data *StorageNew = SNMP_MALLOC_STRUCT(strModTable_data);

				if (StorageNew != NULL) {
					StorageNew->strModTable_request = sa_request;	/* mark as updated */
					StorageNew->strModTable_refs = 1;	/* mark as used */
					StorageNew->strModIdnum = modid;
					if ((StorageNew->strModMajor = list->sc_mlist[i].major) == 0)
						StorageNew->strModType = STRMODTYPE_MODULE;
					else
						StorageNew->strModType = STRMODTYPE_DRIVER;
					if ((StorageNew->strModName = calloc(len + 1, 1)) != NULL) {
						memcpy((char *) StorageNew->strModName, list->sc_mlist[i].name, len);
						StorageNew->strModNameLen = len;
						StorageNew->strModName[len] = '\0';
					}
					StorageNew->strModRowStatus = RS_ACTIVE;
					strModTable_add(StorageNew);
				}
			}
			for (j = 0; j < 4; j++) {
				if (j > 0 && list->sc_mlist[i].mi[j].index == 0)
					continue;
				/* update the StrModInfoTable too */
				for (hciptr = strModInfoTableStorage; hciptr; hciptr = hciptr->next) {
					struct strModInfoTable_data *StorageTmp = hciptr->data;

					if (StorageTmp->strModInfoIndex == j + 1 && StorageTmp->strModNameLen == len && strncmp((char *)StorageTmp->strModName, list->sc_mlist[i].name, len) == 0)
						break;
				}
				if (hciptr) {
					struct strModInfoTable_data *StorageTmp = hciptr->data;

					/* already have an entry for this modid and index, simply refresh the contents */
					StorageTmp->strModInfoTable_request = sa_request;	/* mark as updated */
					StorageTmp->strModInfoTable_refs++;	/* mark as used */
					StorageTmp->strModInfoQueues[0] = 0;
					if (list->sc_mlist[i].mi[j].index & 0x8)
						StorageTmp->strModInfoQueues[0] |= (1 << (8 - STRMODINFOQUEUES_WQ));
					if (list->sc_mlist[i].mi[j].index & 0x4)
						StorageTmp->strModInfoQueues[0] |= (1 << (8 - STRMODINFOQUEUES_RQ));
					if (list->sc_mlist[i].mi[j].index & 0x2)
						StorageTmp->strModInfoQueues[0] |= (1 << (8 - STRMODINFOQUEUES_MUXWQ));
					if (list->sc_mlist[i].mi[j].index & 0x1)
						StorageTmp->strModInfoQueues[0] |= (1 << (8 - STRMODINFOQUEUES_MUXRQ));
					StorageTmp->strModInfoMinpsz = list->sc_mlist[i].mi[j].mi_minpsz;
					StorageTmp->strModInfoMaxpsz = list->sc_mlist[i].mi[j].mi_maxpsz;
					StorageTmp->strModInfoHiwat = list->sc_mlist[i].mi[j].mi_hiwat;
					StorageTmp->strModInfoLowat = list->sc_mlist[i].mi[j].mi_lowat;
					/* leave StorageTmp->strModInfoTraceLevel where it is at */
				} else {
					struct strModInfoTable_data *StorageNew = SNMP_MALLOC_STRUCT(strModInfoTable_data);

					if (StorageNew != NULL) {
						StorageNew->strModInfoTable_request = sa_request;	/* mark as updated */
						StorageNew->strModInfoTable_refs = 1;	/* mark as used */
						if ((StorageNew->strModName = calloc(len + 1, 1)) != NULL) {
							memcpy((char *) StorageNew->strModName, list->sc_mlist[i].name, len);
							StorageNew->strModNameLen = len;
							StorageNew->strModName[len] = '\0';
						}
						StorageNew->strModInfoIndex = j + 1;
						if ((StorageNew->strModInfoQueues = calloc(2, 1)) != NULL) {
							StorageNew->strModInfoQueuesLen = 1;
							StorageNew->strModInfoQueues[0] = 0;
							if (list->sc_mlist[i].mi[j].index & 0x8)
								StorageNew->strModInfoQueues[0] |= (1 << (8 - STRMODINFOQUEUES_WQ));
							if (list->sc_mlist[i].mi[j].index & 0x4)
								StorageNew->strModInfoQueues[0] |= (1 << (8 - STRMODINFOQUEUES_RQ));
							if (list->sc_mlist[i].mi[j].index & 0x2)
								StorageNew->strModInfoQueues[0] |= (1 << (8 - STRMODINFOQUEUES_MUXWQ));
							if (list->sc_mlist[i].mi[j].index & 0x1)
								StorageNew->strModInfoQueues[0] |= (1 << (8 - STRMODINFOQUEUES_MUXRQ));
						}
						StorageNew->strModInfoMinpsz = list->sc_mlist[i].mi[j].mi_minpsz;
						StorageNew->strModInfoMaxpsz = list->sc_mlist[i].mi[j].mi_maxpsz;
						StorageNew->strModInfoHiwat = list->sc_mlist[i].mi[j].mi_hiwat;
						StorageNew->strModInfoLowat = list->sc_mlist[i].mi[j].mi_lowat;
						StorageNew->strModInfoTraceLevel = 0;
						strModInfoTable_add(StorageNew);
					}
				}
				/* update the StrModStatTable too */
				for (hciptr = strModStatTableStorage; hciptr; hciptr = hciptr->next) {
					struct strModStatTable_data *StorageTmp = hciptr->data;

					if (StorageTmp->strModStatIndex == j + 1 && StorageTmp->strModNameLen == len && strncmp((char *)StorageTmp->strModName, list->sc_mlist[i].name, len) == 0)
						break;
				}
				if (hciptr) {
					struct strModStatTable_data *StorageTmp = hciptr->data;

					/* already have an entry for this modid and index, simply refresh the contents */
					StorageTmp->strModStatTable_request = sa_request;	/* mark as updated */
					StorageTmp->strModStatTable_refs++;	/* mark as used */
					StorageTmp->strModStatQueues[0] = 0;
					if (list->sc_mlist[i].mi[j].index & 0x8)
						StorageTmp->strModStatQueues[0] |= (1 << (8 - STRMODSTATQUEUES_WQ));
					if (list->sc_mlist[i].mi[j].index & 0x4)
						StorageTmp->strModStatQueues[0] |= (1 << (8 - STRMODSTATQUEUES_RQ));
					if (list->sc_mlist[i].mi[j].index & 0x2)
						StorageTmp->strModStatQueues[0] |= (1 << (8 - STRMODSTATQUEUES_MUXWQ));
					if (list->sc_mlist[i].mi[j].index & 0x1)
						StorageTmp->strModStatQueues[0] |= (1 << (8 - STRMODSTATQUEUES_MUXRQ));
					StorageTmp->strModStatPcnt = list->sc_mlist[i].ms[j].ms_pcnt;
					StorageTmp->strModStatScnt = list->sc_mlist[i].ms[j].ms_scnt;
					StorageTmp->strModStatOcnt = list->sc_mlist[i].ms[j].ms_ocnt;
					StorageTmp->strModStatCcnt = list->sc_mlist[i].ms[j].ms_ccnt;
					StorageTmp->strModStatAcnt = list->sc_mlist[i].ms[j].ms_acnt;
					StorageTmp->strModStatFlags = list->sc_mlist[i].ms[j].ms_flags;
				} else {
					struct strModStatTable_data *StorageNew = SNMP_MALLOC_STRUCT(strModStatTable_data);

					if (StorageNew != NULL) {
						StorageNew->strModStatTable_request = sa_request;	/* mark as updated */
						StorageNew->strModStatTable_refs = 1;	/* mark as used */
						if ((StorageNew->strModName = calloc(len + 1, 1)) != NULL) {
							memcpy((char *) StorageNew->strModName, list->sc_mlist[i].name, len);
							StorageNew->strModNameLen = len;
							StorageNew->strModName[len] = '\0';
						}
						StorageNew->strModStatIndex = j + 1;
						if ((StorageNew->strModStatQueues = calloc(2, 1)) != NULL) {
							StorageNew->strModStatQueuesLen = 1;
							StorageNew->strModStatQueues[0] = 0;
							if (list->sc_mlist[i].mi[j].index & 0x8)
								StorageNew->strModStatQueues[0] |= (1 << (8 - STRMODSTATQUEUES_WQ));
							if (list->sc_mlist[i].mi[j].index & 0x4)
								StorageNew->strModStatQueues[0] |= (1 << (8 - STRMODSTATQUEUES_RQ));
							if (list->sc_mlist[i].mi[j].index & 0x2)
								StorageNew->strModStatQueues[0] |= (1 << (8 - STRMODSTATQUEUES_MUXWQ));
							if (list->sc_mlist[i].mi[j].index & 0x1)
								StorageNew->strModStatQueues[0] |= (1 << (8 - STRMODSTATQUEUES_MUXRQ));
						}
						StorageNew->strModStatPcnt = list->sc_mlist[i].ms[j].ms_pcnt;
						StorageNew->strModStatScnt = list->sc_mlist[i].ms[j].ms_scnt;
						StorageNew->strModStatOcnt = list->sc_mlist[i].ms[j].ms_ocnt;
						StorageNew->strModStatCcnt = list->sc_mlist[i].ms[j].ms_ccnt;
						StorageNew->strModStatAcnt = list->sc_mlist[i].ms[j].ms_acnt;
						StorageNew->strModStatFlags = list->sc_mlist[i].ms[j].ms_flags;
						strModStatTable_add(StorageNew);
					}
				}
			}
		}
		/* delete unused entries */
		{
			struct strModTable_data *StorageTmp;

			hciptr_next = strModTableStorage;
			while ((hciptr = hciptr_next)) {
				hciptr_next = hciptr->next;
				StorageTmp = hciptr->data;
				if (StorageTmp->strModTable_refs)
					StorageTmp->strModTable_refs = 0;
				else {
					header_complex_extract_entry(&strModTableStorage, hciptr);
					strModTable_destroy(&StorageTmp);
				}
			}
		}
		{
			struct strModInfoTable_data *StorageTmp;

			hciptr_next = strModInfoTableStorage;
			while ((hciptr = hciptr_next)) {
				hciptr_next = hciptr->next;
				StorageTmp = hciptr->data;
				if (StorageTmp->strModInfoTable_refs)
					StorageTmp->strModInfoTable_refs = 0;
				else {
					header_complex_extract_entry(&strModInfoTableStorage, hciptr);
					strModInfoTable_destroy(&StorageTmp);
				}
			}
		}
		{
			struct strModStatTable_data *StorageTmp;

			hciptr_next = strModStatTableStorage;
			while ((hciptr = hciptr_next)) {
				hciptr_next = hciptr->next;
				StorageTmp = hciptr->data;
				if (StorageTmp->strModStatTable_refs)
					StorageTmp->strModStatTable_refs = 0;
				else {
					header_complex_extract_entry(&strModStatTableStorage, hciptr);
					strModStatTable_destroy(&StorageTmp);
				}
			}
		}
		return;
	      error_out:
		snmp_log_perror(__FUNCTION__);
		if (list != NULL)
			free(list);
		if (fd >= 0)
			close(fd);
		return;

	}
}

/**
 * @fn u_char *var_strModTable(struct variable *vp, oid *name, size_t *length, int exact, size_t *var_len, WriteMethod **write_method)
 * @brief locate variables in strModTable.
 *
 * Handle this table separately from the scalar value case.  The workings of this are basically the
 * same as for var_strMIB above.
 */
u_char *
var_strModTable(struct variable *vp, oid * name, size_t *length, int exact, size_t *var_len, WriteMethod ** write_method)
{
	struct strModTable_data *StorageTmp = NULL;
	u_char *rval;

	DEBUGMSGTL(("strMIB", "var_strModTable: Entering...  \n"));
	/* Make sure that the storage data does not need to be refreshed before checking the header. */
	refresh_strModTable(0);
	/* This assumes you have registered all your data properly with header_complex_add() somewhere before this. */
	while ((StorageTmp = header_complex(strModTableStorage, vp, name, length, exact, var_len, write_method)))
		if ((StorageTmp = refresh_strModTable_row(StorageTmp, 0)) || exact)
			break;
	*write_method = NULL;
	*var_len = 0;
	rval = NULL;
	/* This is where we do the value assignments for the mib results. */
	switch (vp->magic) {
	case (u_char) STRMODIDNUM:	/* ReadOnly */
		if (!StorageTmp)
			break;
		*var_len = sizeof(StorageTmp->strModIdnum);
		rval = (u_char *) &StorageTmp->strModIdnum;
		break;
	case (u_char) STRMODTYPE:	/* ReadOnly */
		if (!StorageTmp)
			break;
		*var_len = sizeof(StorageTmp->strModType);
		rval = (u_char *) &StorageTmp->strModType;
		break;
	case (u_char) STRMODMAJOR:	/* ReadOnly */
		if (!StorageTmp)
			break;
		*var_len = sizeof(StorageTmp->strModMajor);
		rval = (u_char *) &StorageTmp->strModMajor;
		break;
	case (u_char) STRMODROWSTATUS:	/* Create */
		*write_method = write_strModRowStatus;
		if (!StorageTmp)
			break;
		*var_len = sizeof(StorageTmp->strModRowStatus);
		rval = (u_char *) &StorageTmp->strModRowStatus;
		break;
	default:
		ERROR_MSG("");
	}
	return (rval);
}

/**
 * @fn void refresh_strModInfoTable_row(struct strModInfoTable_data *StorageTmp, int force)
 * @param StorageTmp the data row to refresh.
 * @param force force refresh if non-zero.
 * @brief refresh the contents of the strModInfoTable row.
 *
 * Normally the values retrieved from the operating system are cached.  However, if a row contains
 * temporal values, such as statistics counters, gauges, timestamps, or other transient columns, it
 * may be necessary to refresh the row on some other basis, but normally only once per request.
 */
struct strModInfoTable_data *
refresh_strModInfoTable_row(struct strModInfoTable_data *StorageTmp, int force)
{
	if (!StorageTmp || (!force && StorageTmp->strModInfoTable_request == sa_request))
		return (StorageTmp);
	/* XXX: update row; delete it and return NULL if the row has disappeared */
	StorageTmp->strModInfoTable_request = sa_request;
	return (StorageTmp);
}

/**
 * @fn void refresh_strModInfoTable(int force)
 * @param force force refresh if non-zero.
 * @brief refresh the scalar values of the strModInfoTable.
 *
 * Normally the values retrieved from the operating system are cached.  When the agent receives a
 * SIGPOLL from an open STREAMS configuration or administrative driver Stream, the STREAMS subsystem
 * indicates to the agent that the cache has been invalidated and that it should reread scalars and
 * tables from the STREAMS subsystem.  This function is used when the agent starts for the first
 * time, or after a SIGPOLL has been received (and a row or column has been requested).
 */
void
refresh_strModInfoTable(int force)
{
	refresh_strModTable(force);
	if (!force && strModInfoTable_refresh == 0)
		return;
	/* XXX: Here, update the table as required... */
	strModInfoTable_refresh = 0;
}

/**
 * @fn u_char *var_strModInfoTable(struct variable *vp, oid *name, size_t *length, int exact, size_t *var_len, WriteMethod **write_method)
 * @brief locate variables in strModInfoTable.
 *
 * Handle this table separately from the scalar value case.  The workings of this are basically the
 * same as for var_strMIB above.
 */
u_char *
var_strModInfoTable(struct variable *vp, oid * name, size_t *length, int exact, size_t *var_len, WriteMethod ** write_method)
{
	struct strModInfoTable_data *StorageTmp = NULL;
	u_char *rval;

	DEBUGMSGTL(("strMIB", "var_strModInfoTable: Entering...  \n"));
	/* Make sure that the storage data does not need to be refreshed before checking the header. */
	refresh_strModInfoTable(0);
	/* This assumes you have registered all your data properly with header_complex_add() somewhere before this. */
	while ((StorageTmp = header_complex(strModInfoTableStorage, vp, name, length, exact, var_len, write_method)))
		if ((StorageTmp = refresh_strModInfoTable_row(StorageTmp, 0)) || exact)
			break;
	*write_method = NULL;
	*var_len = 0;
	rval = NULL;
	/* This is where we do the value assignments for the mib results. */
	switch (vp->magic) {
	case (u_char) STRMODINFOQUEUES:	/* ReadOnly */
		if (!StorageTmp)
			break;
		*var_len = StorageTmp->strModInfoQueuesLen;
		rval = (u_char *) StorageTmp->strModInfoQueues;
		break;
	case (u_char) STRMODINFOMINPSZ:	/* ReadWrite */
		*write_method = write_strModInfoMinpsz;
		if (!StorageTmp)
			break;
		*var_len = sizeof(StorageTmp->strModInfoMinpsz);
		rval = (u_char *) &StorageTmp->strModInfoMinpsz;
		break;
	case (u_char) STRMODINFOMAXPSZ:	/* ReadWrite */
		*write_method = write_strModInfoMaxpsz;
		if (!StorageTmp)
			break;
		*var_len = sizeof(StorageTmp->strModInfoMaxpsz);
		rval = (u_char *) &StorageTmp->strModInfoMaxpsz;
		break;
	case (u_char) STRMODINFOHIWAT:	/* ReadWrite */
		*write_method = write_strModInfoHiwat;
		if (!StorageTmp)
			break;
		*var_len = sizeof(StorageTmp->strModInfoHiwat);
		rval = (u_char *) &StorageTmp->strModInfoHiwat;
		break;
	case (u_char) STRMODINFOLOWAT:	/* ReadWrite */
		*write_method = write_strModInfoLowat;
		if (!StorageTmp)
			break;
		*var_len = sizeof(StorageTmp->strModInfoLowat);
		rval = (u_char *) &StorageTmp->strModInfoLowat;
		break;
	case (u_char) STRMODINFOTRACELEVEL:	/* ReadWrite */
		*write_method = write_strModInfoTraceLevel;
		if (!StorageTmp)
			break;
		*var_len = sizeof(StorageTmp->strModInfoTraceLevel);
		rval = (u_char *) &StorageTmp->strModInfoTraceLevel;
		break;
	default:
		ERROR_MSG("");
	}
	return (rval);
}

/**
 * @fn void refresh_strModStatTable_row(struct strModStatTable_data *StorageTmp, int force)
 * @param StorageTmp the data row to refresh.
 * @param force force refresh if non-zero.
 * @brief refresh the contents of the strModStatTable row.
 *
 * Normally the values retrieved from the operating system are cached.  However, if a row contains
 * temporal values, such as statistics counters, gauges, timestamps, or other transient columns, it
 * may be necessary to refresh the row on some other basis, but normally only once per request.
 */
struct strModStatTable_data *
refresh_strModStatTable_row(struct strModStatTable_data *StorageTmp, int force)
{
	if (!StorageTmp || (!force && StorageTmp->strModStatTable_request == sa_request))
		return (StorageTmp);
	/* XXX: update row; delete it and return NULL if the row has disappeared */
	StorageTmp->strModStatTable_request = sa_request;
	return (StorageTmp);
}

/**
 * @fn void refresh_strModStatTable(int force)
 * @param force force refresh if non-zero.
 * @brief refresh the scalar values of the strModStatTable.
 *
 * Normally the values retrieved from the operating system are cached.  When the agent receives a
 * SIGPOLL from an open STREAMS configuration or administrative driver Stream, the STREAMS subsystem
 * indicates to the agent that the cache has been invalidated and that it should reread scalars and
 * tables from the STREAMS subsystem.  This function is used when the agent starts for the first
 * time, or after a SIGPOLL has been received (and a row or column has been requested).
 */
void
refresh_strModStatTable(int force)
{
	refresh_strModTable(force);
	if (!force && strModStatTable_refresh == 0)
		return;
	/* XXX: Here, update the table as required... */
	strModStatTable_refresh = 0;
}

/**
 * @fn u_char *var_strModStatTable(struct variable *vp, oid *name, size_t *length, int exact, size_t *var_len, WriteMethod **write_method)
 * @brief locate variables in strModStatTable.
 *
 * Handle this table separately from the scalar value case.  The workings of this are basically the
 * same as for var_strMIB above.
 */
u_char *
var_strModStatTable(struct variable *vp, oid * name, size_t *length, int exact, size_t *var_len, WriteMethod ** write_method)
{
	struct strModStatTable_data *StorageTmp = NULL;
	u_char *rval;

	DEBUGMSGTL(("strMIB", "var_strModStatTable: Entering...  \n"));
	/* Make sure that the storage data does not need to be refreshed before checking the header. */
	refresh_strModStatTable(0);
	/* This assumes you have registered all your data properly with header_complex_add() somewhere before this. */
	while ((StorageTmp = header_complex(strModStatTableStorage, vp, name, length, exact, var_len, write_method)))
		if ((StorageTmp = refresh_strModStatTable_row(StorageTmp, 0)) || exact)
			break;
	*write_method = NULL;
	*var_len = 0;
	rval = NULL;
	/* This is where we do the value assignments for the mib results. */
	switch (vp->magic) {
	case (u_char) STRMODSTATQUEUES:	/* ReadOnly */
		if (!StorageTmp)
			break;
		*var_len = StorageTmp->strModStatQueuesLen;
		rval = (u_char *) StorageTmp->strModStatQueues;
		break;
	case (u_char) STRMODSTATPCNT:	/* ReadOnly */
		if (!StorageTmp)
			break;
		*var_len = sizeof(StorageTmp->strModStatPcnt);
		rval = (u_char *) &StorageTmp->strModStatPcnt;
		break;
	case (u_char) STRMODSTATSCNT:	/* ReadOnly */
		if (!StorageTmp)
			break;
		*var_len = sizeof(StorageTmp->strModStatScnt);
		rval = (u_char *) &StorageTmp->strModStatScnt;
		break;
	case (u_char) STRMODSTATOCNT:	/* ReadOnly */
		if (!StorageTmp)
			break;
		*var_len = sizeof(StorageTmp->strModStatOcnt);
		rval = (u_char *) &StorageTmp->strModStatOcnt;
		break;
	case (u_char) STRMODSTATCCNT:	/* ReadOnly */
		if (!StorageTmp)
			break;
		*var_len = sizeof(StorageTmp->strModStatCcnt);
		rval = (u_char *) &StorageTmp->strModStatCcnt;
		break;
	case (u_char) STRMODSTATACNT:	/* ReadOnly */
		if (!StorageTmp)
			break;
		*var_len = sizeof(StorageTmp->strModStatAcnt);
		rval = (u_char *) &StorageTmp->strModStatAcnt;
		break;
	case (u_char) STRMODSTATPRIVATE:	/* ReadOnly */
		if (!StorageTmp)
			break;
		*var_len = StorageTmp->strModStatPrivateLen;
		rval = (u_char *) StorageTmp->strModStatPrivate;
		break;
	case (u_char) STRMODSTATFLAGS:	/* ReadOnly */
		if (!StorageTmp)
			break;
		*var_len = sizeof(StorageTmp->strModStatFlags);
		rval = (u_char *) &StorageTmp->strModStatFlags;
		break;
	default:
		ERROR_MSG("");
	}
	return (rval);
}

/**
 * @fn void refresh_strApshTable_row(struct strApshTable_data *StorageTmp, int force)
 * @param StorageTmp the data row to refresh.
 * @param force force refresh if non-zero.
 * @brief refresh the contents of the strApshTable row.
 *
 * Normally the values retrieved from the operating system are cached.  However, if a row contains
 * temporal values, such as statistics counters, gauges, timestamps, or other transient columns, it
 * may be necessary to refresh the row on some other basis, but normally only once per request.
 */
struct strApshTable_data *
refresh_strApshTable_row(struct strApshTable_data *StorageTmp, int force)
{
	if (!StorageTmp || (!force && StorageTmp->strApshTable_request == sa_request))
		return (StorageTmp);
	/* XXX: update row; delete it and return NULL if the row has disappeared */
	{
		int fd;
		struct strapush sap;
		char *tmp, *ptr;

		if ((fd = open("/dev/streams/clone/sad", O_RDWR)) < 0) {
			snmp_log_perror(__FUNCTION__);
			return (StorageTmp);
		}
		sap.sap_cmd = SAD_GAP;
		sap.sap_major = 0;
		sap.sap_minor = StorageTmp->strApshMinor;
		sap.sap_lastminor = StorageTmp->strApshLastMinor;
		sap.sap_npush = MAXAPUSH;
		sap.sap_anchor = 0;
		memset((char *) sap.sap_module, 0, FMNAMESZ + 1);
		memcpy((char *) sap.sap_module, StorageTmp->strModName, StorageTmp->strModNameLen);

		if (ioctl(fd, SAD_GAP, &sap) < 0) {
			if (errno != ENODEV)
				snmp_log_perror(__FUNCTION__);
			StorageTmp->strApshRowStatus = RS_NOTINSERVICE;
			close(fd);
			return (StorageTmp);
		}
		close(fd);
		if ((tmp = calloc(MAXAPUSH, FMNAMESZ + 1))) {
			int i;

			for (ptr = tmp, i = 0; i < sap.sap_npush; i++)
				ptr += snprintf(ptr, FMNAMESZ + 1, "%s ", sap.sap_list[i]);
			ptr--;
			*ptr = '\0';
			SNMP_FREE(StorageTmp->strApshModules);
			StorageTmp->strApshModules = (uint8_t *) tmp;
			StorageTmp->strApshModulesLen = (ptr - tmp);
		}
	}
	StorageTmp->strApshTable_request = sa_request;
	return (StorageTmp);
}

/**
 * @fn void refresh_strApshTable(int force)
 * @param force force refresh if non-zero.
 * @brief refresh the scalar values of the strApshTable.
 *
 * Normally the values retrieved from the operating system are cached.  When the agent receives a
 * SIGPOLL from an open STREAMS configuration or administrative driver Stream, the STREAMS subsystem
 * indicates to the agent that the cache has been invalidated and that it should reread scalars and
 * tables from the STREAMS subsystem.  This function is used when the agent starts for the first
 * time, or after a SIGPOLL has been received (and a row or column has been requested).
 */
void
refresh_strApshTable(int force)
{
	struct header_complex_index *h, *h_next, *hciptr;
	int fd;

	if (!force && strApshTable_refresh == 0)
		return;
	/* XXX: Here, update the table as required... */
	strApshTable_refresh = 0;
	refresh_strModTable(force);
	if ((fd = open("/dev/streams/clone/sad", O_RDWR)) < 0) {
		snmp_log_perror(__FUNCTION__);
		return;
	}
	for (h = strModTableStorage; h; h = h->next) {
		struct strModTable_data *d = h->data;
		struct strapush sap;
		char *tmp, *ptr;
		int i, minor = 0;

	      next_minor:
		sap.sap_cmd = SAD_LAP;
		sap.sap_major = 0;
		sap.sap_minor = minor;
		sap.sap_lastminor = 0;
		sap.sap_npush = MAXAPUSH;
		sap.sap_anchor = 0;
		memset((char *) sap.sap_module, 0, FMNAMESZ + 1);
		memcpy((char *) sap.sap_module, d->strModName, d->strModNameLen);

		if (ioctl(fd, SAD_LAP, &sap) < 0) {
			if (errno == ENODEV)
				continue;
			snmp_log_perror(__FUNCTION__);
			continue;
		}
		for (hciptr = strApshTableStorage; hciptr; hciptr = hciptr->next) {
			struct strApshTable_data *data = hciptr->data;

			if (d->strModNameLen == data->strModNameLen && !strncmp((char *) d->strModName, (char *) data->strModName, d->strModNameLen)
			    && data->strApshMinor == sap.sap_minor && data->strApshLastMinor == sap.sap_lastminor)
				break;
		}
		if (hciptr) {
			/* have a matching entry, just update it */
			struct strApshTable_data *data = hciptr->data;

			data->strApshTable_request = sa_request;	/* mark as updated */
			data->strApshTable_refs = 1;	/* mark as used */
			if ((tmp = calloc(MAXAPUSH, FMNAMESZ + 1))) {
				for (ptr = tmp, i = 0; i < sap.sap_npush; i++)
					ptr += snprintf(ptr, FMNAMESZ + 1, "%s ", sap.sap_list[i]);
				ptr--;
				*ptr = '\0';
				SNMP_FREE(data->strApshModules);
				data->strApshModules = (uint8_t *) tmp;
				data->strApshModulesLen = (ptr - tmp);
			}
		} else {
			/* don't have matching entry, create one */
			struct strApshTable_data *data = SNMP_MALLOC_STRUCT(strApshTable_data);

			data->strApshTable_request = sa_request;	/* mark as updated */
			data->strApshTable_refs = 1;	/* mark as used */
			memdup(&data->strModName, d->strModName, d->strModNameLen + 1);
			data->strModNameLen = d->strModNameLen;
			data->strModName[data->strModNameLen] = '\0';
			data->strApshMinor = sap.sap_minor;
			data->strApshLastMinor = sap.sap_lastminor;
			if ((tmp = calloc(MAXAPUSH, FMNAMESZ + 1))) {
				for (ptr = tmp, i = 0; i < sap.sap_npush; i++)
					ptr += snprintf(ptr, FMNAMESZ + 1, "%s ", sap.sap_list[i]);
				ptr--;
				*ptr = '\0';
				SNMP_FREE(data->strApshModules);
				data->strApshModules = (uint8_t *) tmp;
				data->strApshModulesLen = (ptr - tmp);
			}
			strApshTable_add(data);
		}
		minor = sap.sap_lastminor + 1;
		goto next_minor;
	}
	close(fd);
	/* delete all of the unused entries */
	h_next = strApshTableStorage;
	while ((h = h_next)) {
		struct strApshTable_data *d = h->data;

		h_next = h->next;
		if (d->strApshTable_refs)
			d->strApshTable_refs = 0;
		else {
			header_complex_extract_entry(&strApshTableStorage, h);
			strApshTable_destroy(&d);
		}
	}
}

/**
 * @fn u_char *var_strApshTable(struct variable *vp, oid *name, size_t *length, int exact, size_t *var_len, WriteMethod **write_method)
 * @brief locate variables in strApshTable.
 *
 * Handle this table separately from the scalar value case.  The workings of this are basically the
 * same as for var_strMIB above.
 */
u_char *
var_strApshTable(struct variable *vp, oid * name, size_t *length, int exact, size_t *var_len, WriteMethod ** write_method)
{
	struct strApshTable_data *StorageTmp = NULL;
	u_char *rval;

	DEBUGMSGTL(("strMIB", "var_strApshTable: Entering...  \n"));
	/* Make sure that the storage data does not need to be refreshed before checking the header. */
	refresh_strApshTable(0);
	/* This assumes you have registered all your data properly with header_complex_add() somewhere before this. */
	while ((StorageTmp = header_complex(strApshTableStorage, vp, name, length, exact, var_len, write_method)))
		if ((StorageTmp = refresh_strApshTable_row(StorageTmp, 0)) || exact)
			break;
	*write_method = NULL;
	*var_len = 0;
	rval = NULL;
	/* This is where we do the value assignments for the mib results. */
	switch (vp->magic) {
	case (u_char) STRAPSHMAJOR:	/* ReadOnly */
		if (!StorageTmp)
			break;
		*var_len = sizeof(StorageTmp->strApshMajor);
		rval = (u_char *) &StorageTmp->strApshMajor;
		break;
	case (u_char) STRAPSHMODULES:	/* Create */
		*write_method = write_strApshModules;
		if (!StorageTmp)
			break;
		*var_len = StorageTmp->strApshModulesLen;
		rval = (u_char *) StorageTmp->strApshModules;
		break;
	case (u_char) STRAPSHROWSTATUS:	/* Create */
		*write_method = write_strApshRowStatus;
		if (!StorageTmp)
			break;
		*var_len = sizeof(StorageTmp->strApshRowStatus);
		rval = (u_char *) &StorageTmp->strApshRowStatus;
		break;
	default:
		ERROR_MSG("");
	}
	return (rval);
}

/**
 * @fn void refresh_strStatsTable_row(struct strStatsTable_data *StorageTmp, int force)
 * @param StorageTmp the data row to refresh.
 * @param force force refresh if non-zero.
 * @brief refresh the contents of the strStatsTable row.
 *
 * Normally the values retrieved from the operating system are cached.  However, if a row contains
 * temporal values, such as statistics counters, gauges, timestamps, or other transient columns, it
 * may be necessary to refresh the row on some other basis, but normally only once per request.
 */
struct strStatsTable_data *
refresh_strStatsTable_row(struct strStatsTable_data *StorageTmp, int force)
{
	if (!StorageTmp || (!force && StorageTmp->strStatsTable_request == sa_request))
		return (StorageTmp);
	/* XXX: update row; delete it and return NULL if the row has disappeared */
	StorageTmp->strStatsTable_request = sa_request;
	return (StorageTmp);
}

/**
 * @fn void refresh_strStatsTable(int force)
 * @param force force refresh if non-zero.
 * @brief refresh the scalar values of the strStatsTable.
 *
 * Normally the values retrieved from the operating system are cached.  When the agent receives a
 * SIGPOLL from an open STREAMS configuration or administrative driver Stream, the STREAMS subsystem
 * indicates to the agent that the cache has been invalidated and that it should reread scalars and
 * tables from the STREAMS subsystem.  This function is used when the agent starts for the first
 * time, or after a SIGPOLL has been received (and a row or column has been requested).
 */
void
refresh_strStatsTable(int force)
{
	if (!force && strStatsTable_refresh == 0)
		return;
	/* XXX: Here, update the table as required... */
	{
		int fd, count, i;
		struct sc_slist *sc = NULL;
		struct header_complex_index *h, *h_next;

		if ((fd = open("/dev/streams/clone/sad", O_RDWR)) < 0) {
			snmp_log_perror("refresh_strStatsTable: open(/dev/streams/clone/sad)");
			return;
		}
		if (ioctl(fd, I_PUSH, "sc") < 0) {
			snmp_log_perror("refresh_strStatsTable: I_PUSH(sc)");
			close(fd);
			return;
		}
		if ((count = ioctl(fd, SC_IOC_STATS, sc)) < 0) {
			snmp_log_perror("refresh_strStatsTable: SC_IOC_STATS");
			close(fd);
			return;
		}
		if ((sc = malloc(sizeof(struct sc_slist) + count * sizeof(struct sc_stat))) == NULL) {
			snmp_log_perror("refresh_strStatsTable: malloc");
			close(fd);
			return;
		}
		sc->sc_nstat = count;
		sc->sc_slist = (struct sc_stat *) (sc + 1);
		if (ioctl(fd, SC_IOC_STATS, sc) < 0) {
			snmp_log_perror("refresh_strStatsTable: SC_IOC_STATS(2nd)");
			close(fd);
			free(sc);
			return;
		}
		close(fd);
		for (i = 0; i < count; i++) {
			for (h = strStatsTableStorage; h; h = h->next) {
				struct strStatsTable_data *d = h->data;

				if (d->strStatsStructure == i + 1)
					break;
			}
			if (h) {
				struct strStatsTable_data *d = h->data;

				/* have an entry */
				d->strStatsTable_request = sa_request;
				d->strStatsTable_refs = 1;
				d->strStatsCurrentAllocs = sc->sc_slist[i].sc_alloc;
				d->strStatsHighWaterMark = sc->sc_slist[i].sc_hiwat;
			} else {
				struct strStatsTable_data *d = SNMP_MALLOC_STRUCT(strStatsTable_data);

				d->strStatsTable_request = sa_request;
				d->strStatsTable_refs = 1;
				d->strStatsStructure = i + 1;
				d->strStatsCurrentAllocs = sc->sc_slist[i].sc_alloc;
				d->strStatsHighWaterMark = sc->sc_slist[i].sc_hiwat;
				strStatsTable_add(d);
			}
		}
		/* delete unused entries */
		for (h_next = strStatsTableStorage; (h = h_next);) {
			struct strStatsTable_data *d = h->data;

			h_next = h->next;
			if (d->strStatsTable_refs)
				d->strStatsTable_refs = 0;
			else {
				header_complex_extract_entry(&strStatsTableStorage, h);
				strStatsTable_destroy(&d);
			}
		}
	}
	strStatsTable_refresh = 0;
}

/**
 * @fn u_char *var_strStatsTable(struct variable *vp, oid *name, size_t *length, int exact, size_t *var_len, WriteMethod **write_method)
 * @brief locate variables in strStatsTable.
 *
 * Handle this table separately from the scalar value case.  The workings of this are basically the
 * same as for var_strMIB above.
 */
u_char *
var_strStatsTable(struct variable *vp, oid * name, size_t *length, int exact, size_t *var_len, WriteMethod ** write_method)
{
	struct strStatsTable_data *StorageTmp = NULL;
	u_char *rval;

	DEBUGMSGTL(("strMIB", "var_strStatsTable: Entering...  \n"));
	/* Make sure that the storage data does not need to be refreshed before checking the header. */
	refresh_strStatsTable(0);
	/* This assumes you have registered all your data properly with header_complex_add() somewhere before this. */
	while ((StorageTmp = header_complex(strStatsTableStorage, vp, name, length, exact, var_len, write_method)))
		if ((StorageTmp = refresh_strStatsTable_row(StorageTmp, 0)) || exact)
			break;
	*write_method = NULL;
	*var_len = 0;
	rval = NULL;
	/* This is where we do the value assignments for the mib results. */
	switch (vp->magic) {
	case (u_char) STRSTATSCURRENTALLOCS:	/* ReadOnly */
		if (!StorageTmp)
			break;
		*var_len = sizeof(StorageTmp->strStatsCurrentAllocs);
		rval = (u_char *) &StorageTmp->strStatsCurrentAllocs;
		break;
	case (u_char) STRSTATSHIGHWATERMARK:	/* ReadOnly */
		if (!StorageTmp)
			break;
		*var_len = sizeof(StorageTmp->strStatsHighWaterMark);
		rval = (u_char *) &StorageTmp->strStatsHighWaterMark;
		break;
	default:
		ERROR_MSG("");
	}
	return (rval);
}

/**
 * @fn int write_strModInfoMinpsz(int action, u_char *var_val, u_char var_val_type, size_t var_val_len, u_char *statP, oid *name, size_t name_len)
 * @param action the stage of the SET operation.
 * @param var_val pointer to the varbind value.
 * @param var_val_type the ASN type.
 * @param var_val_len the length of the varbind value.
 * @param statP static pointer.
 * @param name the varbind OID.
 * @param name_len number of elements in OID.
 * @brief Table row and column write routine.
 */
int
write_strModInfoMinpsz(int action, u_char *var_val, u_char var_val_type, size_t var_val_len, u_char *statP, oid * name, size_t name_len)
{
	static long old_value;
	struct strModInfoTable_data *StorageTmp = NULL;
	size_t newlen = name_len - 15;
	long set_value = *((long *) var_val);

	DEBUGMSGTL(("strMIB", "write_strModInfoMinpsz entering action=%d...  \n", action));
	StorageTmp = header_complex(strModInfoTableStorage, NULL, &name[15], &newlen, 1, NULL, NULL);
	if (StorageTmp == NULL)
		return SNMP_ERR_NOSUCHNAME;	/* remove if you support creation here */
	switch (action) {
	case RESERVE1:
		if (var_val_type != ASN_INTEGER) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strModInfoMinpsz not ASN_INTEGER\n");
			return SNMP_ERR_WRONGTYPE;
		}
		if (var_val_len > sizeof(int32_t)) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strModInfoMinpsz: bad length\n");
			return SNMP_ERR_WRONGLENGTH;
		}
		break;
	case RESERVE2:		/* memory reseveration, final preparation... */
		break;
	case ACTION:		/* The variable has been stored in StorageTmp->strModInfoMinpsz for you to use, and you have just been asked to do something with it.  Note that anything done here
				   must be reversable in the UNDO case */
		old_value = StorageTmp->strModInfoMinpsz;
		StorageTmp->strModInfoMinpsz = set_value;
		break;
	case COMMIT:		/* Things are working well, so it's now safe to make the change permanently.  Make sure that anything done here can't fail! */
		break;
	case UNDO:		/* Back out any changes made in the ACTION case */
		StorageTmp->strModInfoMinpsz = old_value;
		/* fall through */
	case FREE:		/* Release any resources that have been allocated */
		break;
	}
	return SNMP_ERR_NOERROR;
}

/**
 * @fn int write_strModInfoMaxpsz(int action, u_char *var_val, u_char var_val_type, size_t var_val_len, u_char *statP, oid *name, size_t name_len)
 * @param action the stage of the SET operation.
 * @param var_val pointer to the varbind value.
 * @param var_val_type the ASN type.
 * @param var_val_len the length of the varbind value.
 * @param statP static pointer.
 * @param name the varbind OID.
 * @param name_len number of elements in OID.
 * @brief Table row and column write routine.
 */
int
write_strModInfoMaxpsz(int action, u_char *var_val, u_char var_val_type, size_t var_val_len, u_char *statP, oid * name, size_t name_len)
{
	static long old_value;
	struct strModInfoTable_data *StorageTmp = NULL;
	size_t newlen = name_len - 15;
	long set_value = *((long *) var_val);

	DEBUGMSGTL(("strMIB", "write_strModInfoMaxpsz entering action=%d...  \n", action));
	StorageTmp = header_complex(strModInfoTableStorage, NULL, &name[15], &newlen, 1, NULL, NULL);
	if (StorageTmp == NULL)
		return SNMP_ERR_NOSUCHNAME;	/* remove if you support creation here */
	switch (action) {
	case RESERVE1:
		if (var_val_type != ASN_INTEGER) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strModInfoMaxpsz not ASN_INTEGER\n");
			return SNMP_ERR_WRONGTYPE;
		}
		if (var_val_len > sizeof(int32_t)) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strModInfoMaxpsz: bad length\n");
			return SNMP_ERR_WRONGLENGTH;
		}
		break;
	case RESERVE2:		/* memory reseveration, final preparation... */
		break;
	case ACTION:		/* The variable has been stored in StorageTmp->strModInfoMaxpsz for you to use, and you have just been asked to do something with it.  Note that anything done here
				   must be reversable in the UNDO case */
		old_value = StorageTmp->strModInfoMaxpsz;
		StorageTmp->strModInfoMaxpsz = set_value;
		break;
	case COMMIT:		/* Things are working well, so it's now safe to make the change permanently.  Make sure that anything done here can't fail! */
		break;
	case UNDO:		/* Back out any changes made in the ACTION case */
		StorageTmp->strModInfoMaxpsz = old_value;
		/* fall through */
	case FREE:		/* Release any resources that have been allocated */
		break;
	}
	return SNMP_ERR_NOERROR;
}

/**
 * @fn int write_strModInfoHiwat(int action, u_char *var_val, u_char var_val_type, size_t var_val_len, u_char *statP, oid *name, size_t name_len)
 * @param action the stage of the SET operation.
 * @param var_val pointer to the varbind value.
 * @param var_val_type the ASN type.
 * @param var_val_len the length of the varbind value.
 * @param statP static pointer.
 * @param name the varbind OID.
 * @param name_len number of elements in OID.
 * @brief Table row and column write routine.
 */
int
write_strModInfoHiwat(int action, u_char *var_val, u_char var_val_type, size_t var_val_len, u_char *statP, oid * name, size_t name_len)
{
	static ulong old_value;
	struct strModInfoTable_data *StorageTmp = NULL;
	size_t newlen = name_len - 15;
	ulong set_value = *((ulong *) var_val);

	DEBUGMSGTL(("strMIB", "write_strModInfoHiwat entering action=%d...  \n", action));
	StorageTmp = header_complex(strModInfoTableStorage, NULL, &name[15], &newlen, 1, NULL, NULL);
	if (StorageTmp == NULL)
		return SNMP_ERR_NOSUCHNAME;	/* remove if you support creation here */
	switch (action) {
	case RESERVE1:
		if (var_val_type != ASN_UNSIGNED) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strModInfoHiwat not ASN_UNSIGNED\n");
			return SNMP_ERR_WRONGTYPE;
		}
		if (var_val_len > sizeof(uint32_t)) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strModInfoHiwat: bad length\n");
			return SNMP_ERR_WRONGLENGTH;
		}
		break;
	case RESERVE2:		/* memory reseveration, final preparation... */
		break;
	case ACTION:		/* The variable has been stored in StorageTmp->strModInfoHiwat for you to use, and you have just been asked to do something with it.  Note that anything done here must 
				   be reversable in the UNDO case */
		old_value = StorageTmp->strModInfoHiwat;
		StorageTmp->strModInfoHiwat = set_value;
		break;
	case COMMIT:		/* Things are working well, so it's now safe to make the change permanently.  Make sure that anything done here can't fail! */
		break;
	case UNDO:		/* Back out any changes made in the ACTION case */
		StorageTmp->strModInfoHiwat = old_value;
		/* fall through */
	case FREE:		/* Release any resources that have been allocated */
		break;
	}
	return SNMP_ERR_NOERROR;
}

/**
 * @fn int write_strModInfoLowat(int action, u_char *var_val, u_char var_val_type, size_t var_val_len, u_char *statP, oid *name, size_t name_len)
 * @param action the stage of the SET operation.
 * @param var_val pointer to the varbind value.
 * @param var_val_type the ASN type.
 * @param var_val_len the length of the varbind value.
 * @param statP static pointer.
 * @param name the varbind OID.
 * @param name_len number of elements in OID.
 * @brief Table row and column write routine.
 */
int
write_strModInfoLowat(int action, u_char *var_val, u_char var_val_type, size_t var_val_len, u_char *statP, oid * name, size_t name_len)
{
	static ulong old_value;
	struct strModInfoTable_data *StorageTmp = NULL;
	size_t newlen = name_len - 15;
	ulong set_value = *((ulong *) var_val);

	DEBUGMSGTL(("strMIB", "write_strModInfoLowat entering action=%d...  \n", action));
	StorageTmp = header_complex(strModInfoTableStorage, NULL, &name[15], &newlen, 1, NULL, NULL);
	if (StorageTmp == NULL)
		return SNMP_ERR_NOSUCHNAME;	/* remove if you support creation here */
	switch (action) {
	case RESERVE1:
		if (var_val_type != ASN_UNSIGNED) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strModInfoLowat not ASN_UNSIGNED\n");
			return SNMP_ERR_WRONGTYPE;
		}
		if (var_val_len > sizeof(uint32_t)) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strModInfoLowat: bad length\n");
			return SNMP_ERR_WRONGLENGTH;
		}
		break;
	case RESERVE2:		/* memory reseveration, final preparation... */
		break;
	case ACTION:		/* The variable has been stored in StorageTmp->strModInfoLowat for you to use, and you have just been asked to do something with it.  Note that anything done here must 
				   be reversable in the UNDO case */
		old_value = StorageTmp->strModInfoLowat;
		StorageTmp->strModInfoLowat = set_value;
		break;
	case COMMIT:		/* Things are working well, so it's now safe to make the change permanently.  Make sure that anything done here can't fail! */
		break;
	case UNDO:		/* Back out any changes made in the ACTION case */
		StorageTmp->strModInfoLowat = old_value;
		/* fall through */
	case FREE:		/* Release any resources that have been allocated */
		break;
	}
	return SNMP_ERR_NOERROR;
}

/**
 * @fn int write_strModInfoTraceLevel(int action, u_char *var_val, u_char var_val_type, size_t var_val_len, u_char *statP, oid *name, size_t name_len)
 * @param action the stage of the SET operation.
 * @param var_val pointer to the varbind value.
 * @param var_val_type the ASN type.
 * @param var_val_len the length of the varbind value.
 * @param statP static pointer.
 * @param name the varbind OID.
 * @param name_len number of elements in OID.
 * @brief Table row and column write routine.
 */
int
write_strModInfoTraceLevel(int action, u_char *var_val, u_char var_val_type, size_t var_val_len, u_char *statP, oid * name, size_t name_len)
{
	static ulong old_value;
	struct strModInfoTable_data *StorageTmp = NULL;
	size_t newlen = name_len - 15;
	ulong set_value = *((ulong *) var_val);

	DEBUGMSGTL(("strMIB", "write_strModInfoTraceLevel entering action=%d...  \n", action));
	StorageTmp = header_complex(strModInfoTableStorage, NULL, &name[15], &newlen, 1, NULL, NULL);
	if (StorageTmp == NULL)
		return SNMP_ERR_NOSUCHNAME;	/* remove if you support creation here */
	switch (action) {
	case RESERVE1:
		if (var_val_type != ASN_UNSIGNED) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strModInfoTraceLevel not ASN_UNSIGNED\n");
			return SNMP_ERR_WRONGTYPE;
		}
		if (var_val_len > sizeof(uint32_t)) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strModInfoTraceLevel: bad length\n");
			return SNMP_ERR_WRONGLENGTH;
		}
		/* Note: ranges 0..255 */
		if ((0 > set_value || set_value > 255)) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strModInfoTraceLevel: bad value\n");
			return SNMP_ERR_WRONGVALUE;
		}
		break;
	case RESERVE2:		/* memory reseveration, final preparation... */
		break;
	case ACTION:		/* The variable has been stored in StorageTmp->strModInfoTraceLevel for you to use, and you have just been asked to do something with it.  Note that anything done here 
				   must be reversable in the UNDO case */
		old_value = StorageTmp->strModInfoTraceLevel;
		StorageTmp->strModInfoTraceLevel = set_value;
		break;
	case COMMIT:		/* Things are working well, so it's now safe to make the change permanently.  Make sure that anything done here can't fail! */
		break;
	case UNDO:		/* Back out any changes made in the ACTION case */
		StorageTmp->strModInfoTraceLevel = old_value;
		/* fall through */
	case FREE:		/* Release any resources that have been allocated */
		break;
	}
	return SNMP_ERR_NOERROR;
}

/**
 * @fn int write_strApshModules(int action, u_char *var_val, u_char var_val_type, size_t var_val_len, u_char *statP, oid *name, size_t name_len)
 * @param action the stage of the SET operation.
 * @param var_val pointer to the varbind value.
 * @param var_val_type the ASN type.
 * @param var_val_len the length of the varbind value.
 * @param statP static pointer.
 * @param name the varbind OID.
 * @param name_len number of elements in OID.
 * @brief Table row and column write routine.
 */
int
write_strApshModules(int action, u_char *var_val, u_char var_val_type, size_t var_val_len, u_char *statP, oid * name, size_t name_len)
{
	static uint8_t *old_value;
	struct strApshTable_data *StorageTmp = NULL;
	size_t newlen = name_len - 15;
	static size_t old_length = 0;
	static uint8_t *string = NULL;

	DEBUGMSGTL(("strMIB", "write_strApshModules entering action=%d...  \n", action));
	StorageTmp = header_complex(strApshTableStorage, NULL, &name[15], &newlen, 1, NULL, NULL);
	switch (action) {
	case RESERVE1:
		string = NULL;
		if (StorageTmp != NULL && statP == NULL) {
			/* have row but no column */
			switch (StorageTmp->strApshRowStatus) {
			case RS_ACTIVE:
				/* cannot create non-existent column while active */
				snmp_log(MY_FACILITY(LOG_NOTICE), "write to strApshModules: but column non-existent\n");
				return SNMP_ERR_INCONSISTENTVALUE;
			case RS_NOTINSERVICE:
			case RS_NOTREADY:
				/* assume column can be created */
				break;
			}
		}
		if (var_val_type != ASN_OCTET_STR) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strApshModules not ASN_OCTET_STR\n");
			return SNMP_ERR_WRONGTYPE;
		}
		/* Note: ranges 0..255 */
		if (var_val_len > SPRINT_MAX_LEN || ((0 > var_val_len || var_val_len > 255))) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strApshModules: bad length\n");
			return SNMP_ERR_WRONGLENGTH;
		}
		break;
	case RESERVE2:		/* memory reseveration, final preparation... */
		if ((string = malloc(var_val_len + 1)) == NULL)
			return SNMP_ERR_RESOURCEUNAVAILABLE;
		memcpy((void *) string, (void *) var_val, var_val_len);
		string[var_val_len] = 0;
		break;
	case ACTION:		/* The variable has been stored in StorageTmp->strApshModules for you to use, and you have just been asked to do something with it.  Note that anything done here must
				   be reversable in the UNDO case */
		if (StorageTmp == NULL)
			return SNMP_ERR_NOSUCHNAME;
		old_value = StorageTmp->strApshModules;
		old_length = StorageTmp->strApshModulesLen;
		StorageTmp->strApshModules = string;
		StorageTmp->strApshModulesLen = var_val_len;
		break;
	case COMMIT:		/* Things are working well, so it's now safe to make the change permanently.  Make sure that anything done here can't fail! */
		SNMP_FREE(old_value);
		old_length = 0;
		string = NULL;
		break;
	case UNDO:		/* Back out any changes made in the ACTION case */
		StorageTmp->strApshModules = old_value;
		StorageTmp->strApshModulesLen = old_length;
		/* fall through */
	case FREE:		/* Release any resources that have been allocated */
		SNMP_FREE(string);
		break;
	}
	return SNMP_ERR_NOERROR;
}

/**
 * @fn int write_strCltime(int action, u_char *var_val, u_char var_val_type, size_t var_val_len, u_char *statP, oid *name, size_t name_len)
 * @param action the stage of the SET operation.
 * @param var_val pointer to the varbind value.
 * @param var_val_type the ASN type.
 * @param var_val_len the length of the varbind value.
 * @param statP static pointer.
 * @param name the varbind OID.
 * @param name_len number of elements in OID.
 * @brief Scalar write routine.
 */
int
write_strCltime(int action, u_char *var_val, u_char var_val_type, size_t var_val_len, u_char *statP, oid * name, size_t name_len)
{
	static long old_value;
	struct strMIB_data *StorageTmp = NULL;
	long set_value = *((long *) var_val);

	DEBUGMSGTL(("strMIB", "write_strCltime entering action=%d...  \n", action));
	if ((StorageTmp = strMIBStorage) == NULL)
		return SNMP_ERR_NOSUCHNAME;	/* remove if you support creation here */
	switch (action) {
	case RESERVE1:
		if (var_val_type != ASN_INTEGER) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strCltime not ASN_INTEGER\n");
			return SNMP_ERR_WRONGTYPE;
		}
		if (var_val_len > sizeof(int32_t)) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strCltime: bad length\n");
			return SNMP_ERR_WRONGLENGTH;
		}
		/* Note: default value 1500 */
		/* Note: ranges 0..2147483647 */
		if ((0 > set_value || set_value > 2147483647)) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strCltime: bad value\n");
			return SNMP_ERR_WRONGVALUE;
		}
		break;
	case RESERVE2:		/* memory reseveration, final preparation... */
		break;
	case ACTION:		/* The variable has been stored in set_value for you to use, and you have just been asked to do something with it.  Note that anything done here must be reversable in
				   the UNDO case */
		old_value = StorageTmp->strCltime;
		StorageTmp->strCltime = set_value;
		break;
	case COMMIT:		/* Things are working well, so it's now safe to make the change permanently.  Make sure that anything done here can't fail! */
	{
		FILE *file;
		ulong value = StorageTmp->strCltime * 10;	/* centiseconds to milliseconds */

		if ((file = fopen("/proc/sys/streams/cltime", "w")) == NULL || fprintf(file, "%lu\n", value) < 0) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strCltime: commit failed\n");
			return SNMP_ERR_COMMITFAILED;
		}
		if (file)
			fclose(file);
		break;
	}
	case UNDO:		/* Back out any changes made in the ACTION case */
		StorageTmp->strCltime = old_value;
		/* fall through */
	case FREE:		/* Release any resources that have been allocated */
		break;
	}
	return SNMP_ERR_NOERROR;
}

/**
 * @fn int write_strMaxApush(int action, u_char *var_val, u_char var_val_type, size_t var_val_len, u_char *statP, oid *name, size_t name_len)
 * @param action the stage of the SET operation.
 * @param var_val pointer to the varbind value.
 * @param var_val_type the ASN type.
 * @param var_val_len the length of the varbind value.
 * @param statP static pointer.
 * @param name the varbind OID.
 * @param name_len number of elements in OID.
 * @brief Scalar write routine.
 */
int
write_strMaxApush(int action, u_char *var_val, u_char var_val_type, size_t var_val_len, u_char *statP, oid * name, size_t name_len)
{
	static ulong old_value;
	struct strMIB_data *StorageTmp = NULL;
	ulong set_value = *((ulong *) var_val);

	DEBUGMSGTL(("strMIB", "write_strMaxApush entering action=%d...  \n", action));
	if ((StorageTmp = strMIBStorage) == NULL)
		return SNMP_ERR_NOSUCHNAME;	/* remove if you support creation here */
	switch (action) {
	case RESERVE1:
		if (var_val_type != ASN_UNSIGNED) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strMaxApush not ASN_UNSIGNED\n");
			return SNMP_ERR_WRONGTYPE;
		}
		if (var_val_len > sizeof(uint32_t)) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strMaxApush: bad length\n");
			return SNMP_ERR_WRONGLENGTH;
		}
		/* Note: default value 8 */
		break;
	case RESERVE2:		/* memory reseveration, final preparation... */
		break;
	case ACTION:		/* The variable has been stored in set_value for you to use, and you have just been asked to do something with it.  Note that anything done here must be reversable in
				   the UNDO case */
		old_value = StorageTmp->strMaxApush;
		StorageTmp->strMaxApush = set_value;
		break;
	case COMMIT:		/* Things are working well, so it's now safe to make the change permanently.  Make sure that anything done here can't fail! */
	{
		FILE *file;
		ulong value = StorageTmp->strMaxApush;

		if ((file = fopen("/proc/sys/streams/max_apush", "w")) == NULL || fprintf(file, "%lu\n", value) < 0) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strMaxApush: commit failed\n");
			return SNMP_ERR_COMMITFAILED;
		}
		if (file)
			fclose(file);
		break;
	}
	case UNDO:		/* Back out any changes made in the ACTION case */
		StorageTmp->strMaxApush = old_value;
		/* fall through */
	case FREE:		/* Release any resources that have been allocated */
		break;
	}
	return SNMP_ERR_NOERROR;
}

/**
 * @fn int write_strMaxMblk(int action, u_char *var_val, u_char var_val_type, size_t var_val_len, u_char *statP, oid *name, size_t name_len)
 * @param action the stage of the SET operation.
 * @param var_val pointer to the varbind value.
 * @param var_val_type the ASN type.
 * @param var_val_len the length of the varbind value.
 * @param statP static pointer.
 * @param name the varbind OID.
 * @param name_len number of elements in OID.
 * @brief Scalar write routine.
 */
int
write_strMaxMblk(int action, u_char *var_val, u_char var_val_type, size_t var_val_len, u_char *statP, oid * name, size_t name_len)
{
	static ulong old_value;
	struct strMIB_data *StorageTmp = NULL;
	ulong set_value = *((ulong *) var_val);

	DEBUGMSGTL(("strMIB", "write_strMaxMblk entering action=%d...  \n", action));
	if ((StorageTmp = strMIBStorage) == NULL)
		return SNMP_ERR_NOSUCHNAME;	/* remove if you support creation here */
	switch (action) {
	case RESERVE1:
		if (var_val_type != ASN_UNSIGNED) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strMaxMblk not ASN_UNSIGNED\n");
			return SNMP_ERR_WRONGTYPE;
		}
		if (var_val_len > sizeof(uint32_t)) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strMaxMblk: bad length\n");
			return SNMP_ERR_WRONGLENGTH;
		}
		/* Note: default value 10 */
		break;
	case RESERVE2:		/* memory reseveration, final preparation... */
		break;
	case ACTION:		/* The variable has been stored in set_value for you to use, and you have just been asked to do something with it.  Note that anything done here must be reversable in
				   the UNDO case */
		old_value = StorageTmp->strMaxMblk;
		StorageTmp->strMaxMblk = set_value;
		break;
	case COMMIT:		/* Things are working well, so it's now safe to make the change permanently.  Make sure that anything done here can't fail! */
	{
		FILE *file;
		ulong value = StorageTmp->strMaxMblk;

		if ((file = fopen("/proc/sys/streams/max_mblk", "w")) == NULL || fprintf(file, "%lu\n", value) < 0) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strMaxMblk: commit failed\n");
			return SNMP_ERR_COMMITFAILED;
		}
		if (file)
			fclose(file);
		break;
	}
	case UNDO:		/* Back out any changes made in the ACTION case */
		StorageTmp->strMaxMblk = old_value;
		/* fall through */
	case FREE:		/* Release any resources that have been allocated */
		break;
	}
	return SNMP_ERR_NOERROR;
}

/**
 * @fn int write_strMaxStramod(int action, u_char *var_val, u_char var_val_type, size_t var_val_len, u_char *statP, oid *name, size_t name_len)
 * @param action the stage of the SET operation.
 * @param var_val pointer to the varbind value.
 * @param var_val_type the ASN type.
 * @param var_val_len the length of the varbind value.
 * @param statP static pointer.
 * @param name the varbind OID.
 * @param name_len number of elements in OID.
 * @brief Scalar write routine.
 */
int
write_strMaxStramod(int action, u_char *var_val, u_char var_val_type, size_t var_val_len, u_char *statP, oid * name, size_t name_len)
{
	static ulong old_value;
	struct strMIB_data *StorageTmp = NULL;
	ulong set_value = *((ulong *) var_val);

	DEBUGMSGTL(("strMIB", "write_strMaxStramod entering action=%d...  \n", action));
	if ((StorageTmp = strMIBStorage) == NULL)
		return SNMP_ERR_NOSUCHNAME;	/* remove if you support creation here */
	switch (action) {
	case RESERVE1:
		if (var_val_type != ASN_UNSIGNED) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strMaxStramod not ASN_UNSIGNED\n");
			return SNMP_ERR_WRONGTYPE;
		}
		if (var_val_len > sizeof(uint32_t)) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strMaxStramod: bad length\n");
			return SNMP_ERR_WRONGLENGTH;
		}
		/* Note: default value 8 */
		break;
	case RESERVE2:		/* memory reseveration, final preparation... */
		break;
	case ACTION:		/* The variable has been stored in set_value for you to use, and you have just been asked to do something with it.  Note that anything done here must be reversable in
				   the UNDO case */
		old_value = StorageTmp->strMaxStramod;
		StorageTmp->strMaxStramod = set_value;
		break;
	case COMMIT:		/* Things are working well, so it's now safe to make the change permanently.  Make sure that anything done here can't fail! */
	{
		FILE *file;
		ulong value = StorageTmp->strMaxStramod;

		if ((file = fopen("/proc/sys/streams/max_stramod", "w")) == NULL || fprintf(file, "%lu\n", value) < 0) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strMaxStramod: commit failed\n");
			return SNMP_ERR_COMMITFAILED;
		}
		break;
	}
	case UNDO:		/* Back out any changes made in the ACTION case */
		StorageTmp->strMaxStramod = old_value;
		/* fall through */
	case FREE:		/* Release any resources that have been allocated */
		break;
	}
	return SNMP_ERR_NOERROR;
}

/**
 * @fn int write_strMsgPriority(int action, u_char *var_val, u_char var_val_type, size_t var_val_len, u_char *statP, oid *name, size_t name_len)
 * @param action the stage of the SET operation.
 * @param var_val pointer to the varbind value.
 * @param var_val_type the ASN type.
 * @param var_val_len the length of the varbind value.
 * @param statP static pointer.
 * @param name the varbind OID.
 * @param name_len number of elements in OID.
 * @brief Scalar write routine.
 */
int
write_strMsgPriority(int action, u_char *var_val, u_char var_val_type, size_t var_val_len, u_char *statP, oid * name, size_t name_len)
{
	static long old_value;
	struct strMIB_data *StorageTmp = NULL;
	long set_value = *((long *) var_val);

	DEBUGMSGTL(("strMIB", "write_strMsgPriority entering action=%d...  \n", action));
	if ((StorageTmp = strMIBStorage) == NULL)
		return SNMP_ERR_NOSUCHNAME;	/* remove if you support creation here */
	switch (action) {
	case RESERVE1:
		if (var_val_type != ASN_INTEGER) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strMsgPriority not ASN_INTEGER\n");
			return SNMP_ERR_WRONGTYPE;
		}
		if (var_val_len > sizeof(int32_t)) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strMsgPriority: bad length\n");
			return SNMP_ERR_WRONGLENGTH;
		}
		/* Note: default value 2 */
		switch (set_value) {
		case TV_TRUE:
		case TV_FALSE:
			break;
		default:
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strMsgPriority: bad value\n");
			return SNMP_ERR_WRONGVALUE;
		}
		break;
	case RESERVE2:		/* memory reseveration, final preparation... */
		break;
	case ACTION:		/* The variable has been stored in set_value for you to use, and you have just been asked to do something with it.  Note that anything done here must be reversable in
				   the UNDO case */
		old_value = StorageTmp->strMsgPriority;
		StorageTmp->strMsgPriority = set_value;
		break;
	case COMMIT:		/* Things are working well, so it's now safe to make the change permanently.  Make sure that anything done here can't fail! */
	{
		FILE *file;
		int value = (StorageTmp->strMsgPriority == TV_TRUE) ? 1 : 0;

		if ((file = fopen("/proc/sys/streams/msg_priority", "w")) == NULL || fprintf(file, "%d\n", value) < 0) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strMsgPriority: commit failed\n");
			return SNMP_ERR_COMMITFAILED;
		}
		if (file)
			fclose(file);
		break;
	}
	case UNDO:		/* Back out any changes made in the ACTION case */
		StorageTmp->strMsgPriority = old_value;
		/* fall through */
	case FREE:		/* Release any resources that have been allocated */
		break;
	}
	return SNMP_ERR_NOERROR;
}

/**
 * @fn int write_strNstrmsgs(int action, u_char *var_val, u_char var_val_type, size_t var_val_len, u_char *statP, oid *name, size_t name_len)
 * @param action the stage of the SET operation.
 * @param var_val pointer to the varbind value.
 * @param var_val_type the ASN type.
 * @param var_val_len the length of the varbind value.
 * @param statP static pointer.
 * @param name the varbind OID.
 * @param name_len number of elements in OID.
 * @brief Scalar write routine.
 */
int
write_strNstrmsgs(int action, u_char *var_val, u_char var_val_type, size_t var_val_len, u_char *statP, oid * name, size_t name_len)
{
	static ulong old_value;
	struct strMIB_data *StorageTmp = NULL;
	ulong set_value = *((ulong *) var_val);

	DEBUGMSGTL(("strMIB", "write_strNstrmsgs entering action=%d...  \n", action));
	if ((StorageTmp = strMIBStorage) == NULL)
		return SNMP_ERR_NOSUCHNAME;	/* remove if you support creation here */
	switch (action) {
	case RESERVE1:
		if (var_val_type != ASN_UNSIGNED) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strNstrmsgs not ASN_UNSIGNED\n");
			return SNMP_ERR_WRONGTYPE;
		}
		if (var_val_len > sizeof(uint32_t)) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strNstrmsgs: bad length\n");
			return SNMP_ERR_WRONGLENGTH;
		}
		/* Note: default value 4096 */
		break;
	case RESERVE2:		/* memory reseveration, final preparation... */
		break;
	case ACTION:		/* The variable has been stored in set_value for you to use, and you have just been asked to do something with it.  Note that anything done here must be reversable in
				   the UNDO case */
		old_value = StorageTmp->strNstrmsgs;
		StorageTmp->strNstrmsgs = set_value;
		break;
	case COMMIT:		/* Things are working well, so it's now safe to make the change permanently.  Make sure that anything done here can't fail! */
	{
		FILE *file;
		ulong value = StorageTmp->strNstrmsgs;

		if ((file = fopen("/proc/sys/streams/nstrmsgs", "w")) == NULL || fprintf(file, "%lu\n", value) < 0) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strNstrmsgs: commit failed\n");
			return SNMP_ERR_COMMITFAILED;
		}
		if (file)
			fclose(file);
		break;
	}
	case UNDO:		/* Back out any changes made in the ACTION case */
		StorageTmp->strNstrmsgs = old_value;
		/* fall through */
	case FREE:		/* Release any resources that have been allocated */
		break;
	}
	return SNMP_ERR_NOERROR;
}

/**
 * @fn int write_strNstrpush(int action, u_char *var_val, u_char var_val_type, size_t var_val_len, u_char *statP, oid *name, size_t name_len)
 * @param action the stage of the SET operation.
 * @param var_val pointer to the varbind value.
 * @param var_val_type the ASN type.
 * @param var_val_len the length of the varbind value.
 * @param statP static pointer.
 * @param name the varbind OID.
 * @param name_len number of elements in OID.
 * @brief Scalar write routine.
 */
int
write_strNstrpush(int action, u_char *var_val, u_char var_val_type, size_t var_val_len, u_char *statP, oid * name, size_t name_len)
{
	static ulong old_value;
	struct strMIB_data *StorageTmp = NULL;
	ulong set_value = *((ulong *) var_val);

	DEBUGMSGTL(("strMIB", "write_strNstrpush entering action=%d...  \n", action));
	if ((StorageTmp = strMIBStorage) == NULL)
		return SNMP_ERR_NOSUCHNAME;	/* remove if you support creation here */
	switch (action) {
	case RESERVE1:
		if (var_val_type != ASN_UNSIGNED) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strNstrpush not ASN_UNSIGNED\n");
			return SNMP_ERR_WRONGTYPE;
		}
		if (var_val_len > sizeof(uint32_t)) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strNstrpush: bad length\n");
			return SNMP_ERR_WRONGLENGTH;
		}
		/* Note: default value 64 */
		break;
	case RESERVE2:		/* memory reseveration, final preparation... */
		break;
	case ACTION:		/* The variable has been stored in set_value for you to use, and you have just been asked to do something with it.  Note that anything done here must be reversable in
				   the UNDO case */
		old_value = StorageTmp->strNstrpush;
		StorageTmp->strNstrpush = set_value;
		break;
	case COMMIT:		/* Things are working well, so it's now safe to make the change permanently.  Make sure that anything done here can't fail! */
	{
		FILE *file;
		ulong value = StorageTmp->strNstrpush;

		if ((file = fopen("/proc/sys/streams/nstrpush", "w")) == NULL || fprintf(file, "%lu\n", value) < 0) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strNstrpush: commit failed\n");
			return SNMP_ERR_COMMITFAILED;
		}
		if (file)
			fclose(file);
		break;
	}
	case UNDO:		/* Back out any changes made in the ACTION case */
		StorageTmp->strNstrpush = old_value;
		/* fall through */
	case FREE:		/* Release any resources that have been allocated */
		break;
	}
	return SNMP_ERR_NOERROR;
}

/**
 * @fn int write_strHiwat(int action, u_char *var_val, u_char var_val_type, size_t var_val_len, u_char *statP, oid *name, size_t name_len)
 * @param action the stage of the SET operation.
 * @param var_val pointer to the varbind value.
 * @param var_val_type the ASN type.
 * @param var_val_len the length of the varbind value.
 * @param statP static pointer.
 * @param name the varbind OID.
 * @param name_len number of elements in OID.
 * @brief Scalar write routine.
 */
int
write_strHiwat(int action, u_char *var_val, u_char var_val_type, size_t var_val_len, u_char *statP, oid * name, size_t name_len)
{
	static ulong old_value;
	struct strMIB_data *StorageTmp = NULL;
	ulong set_value = *((ulong *) var_val);

	DEBUGMSGTL(("strMIB", "write_strHiwat entering action=%d...  \n", action));
	if ((StorageTmp = strMIBStorage) == NULL)
		return SNMP_ERR_NOSUCHNAME;	/* remove if you support creation here */
	switch (action) {
	case RESERVE1:
		if (var_val_type != ASN_UNSIGNED) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strHiwat not ASN_UNSIGNED\n");
			return SNMP_ERR_WRONGTYPE;
		}
		if (var_val_len > sizeof(uint32_t)) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strHiwat: bad length\n");
			return SNMP_ERR_WRONGLENGTH;
		}
		/* Note: default value 5120 */
		break;
	case RESERVE2:		/* memory reseveration, final preparation... */
		break;
	case ACTION:		/* The variable has been stored in set_value for you to use, and you have just been asked to do something with it.  Note that anything done here must be reversable in
				   the UNDO case */
		old_value = StorageTmp->strHiwat;
		StorageTmp->strHiwat = set_value;
		break;
	case COMMIT:		/* Things are working well, so it's now safe to make the change permanently.  Make sure that anything done here can't fail! */
	{
		FILE *file;
		ulong value = StorageTmp->strHiwat;

		if ((file = fopen("/proc/sys/streams/hiwat", "w")) == NULL || fprintf(file, "%lu\n", value) < 0) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strHiwat: commit failed\n");
			return SNMP_ERR_COMMITFAILED;
		}
		if (file)
			fclose(file);
		break;
	}
	case UNDO:		/* Back out any changes made in the ACTION case */
		StorageTmp->strHiwat = old_value;
		/* fall through */
	case FREE:		/* Release any resources that have been allocated */
		break;
	}
	return SNMP_ERR_NOERROR;
}

/**
 * @fn int write_strLowat(int action, u_char *var_val, u_char var_val_type, size_t var_val_len, u_char *statP, oid *name, size_t name_len)
 * @param action the stage of the SET operation.
 * @param var_val pointer to the varbind value.
 * @param var_val_type the ASN type.
 * @param var_val_len the length of the varbind value.
 * @param statP static pointer.
 * @param name the varbind OID.
 * @param name_len number of elements in OID.
 * @brief Scalar write routine.
 */
int
write_strLowat(int action, u_char *var_val, u_char var_val_type, size_t var_val_len, u_char *statP, oid * name, size_t name_len)
{
	static ulong old_value;
	struct strMIB_data *StorageTmp = NULL;
	ulong set_value = *((ulong *) var_val);

	DEBUGMSGTL(("strMIB", "write_strLowat entering action=%d...  \n", action));
	if ((StorageTmp = strMIBStorage) == NULL)
		return SNMP_ERR_NOSUCHNAME;	/* remove if you support creation here */
	switch (action) {
	case RESERVE1:
		if (var_val_type != ASN_UNSIGNED) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strLowat not ASN_UNSIGNED\n");
			return SNMP_ERR_WRONGTYPE;
		}
		if (var_val_len > sizeof(uint32_t)) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strLowat: bad length\n");
			return SNMP_ERR_WRONGLENGTH;
		}
		/* Note: default value 1024 */
		break;
	case RESERVE2:		/* memory reseveration, final preparation... */
		break;
	case ACTION:		/* The variable has been stored in set_value for you to use, and you have just been asked to do something with it.  Note that anything done here must be reversable in
				   the UNDO case */
		old_value = StorageTmp->strLowat;
		StorageTmp->strLowat = set_value;
		break;
	case COMMIT:		/* Things are working well, so it's now safe to make the change permanently.  Make sure that anything done here can't fail! */
	{
		FILE *file;
		ulong value = StorageTmp->strLowat;

		if ((file = fopen("/proc/sys/streams/lowat", "w")) == NULL || fprintf(file, "%lu\n", value) < 0) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strLowat: commit failed\n");
			return SNMP_ERR_COMMITFAILED;
		}
		if (file)
			fclose(file);
		break;
	}
	case UNDO:		/* Back out any changes made in the ACTION case */
		StorageTmp->strLowat = old_value;
		/* fall through */
	case FREE:		/* Release any resources that have been allocated */
		break;
	}
	return SNMP_ERR_NOERROR;
}

/**
 * @fn int write_strMaxpsz(int action, u_char *var_val, u_char var_val_type, size_t var_val_len, u_char *statP, oid *name, size_t name_len)
 * @param action the stage of the SET operation.
 * @param var_val pointer to the varbind value.
 * @param var_val_type the ASN type.
 * @param var_val_len the length of the varbind value.
 * @param statP static pointer.
 * @param name the varbind OID.
 * @param name_len number of elements in OID.
 * @brief Scalar write routine.
 */
int
write_strMaxpsz(int action, u_char *var_val, u_char var_val_type, size_t var_val_len, u_char *statP, oid * name, size_t name_len)
{
	static long old_value;
	struct strMIB_data *StorageTmp = NULL;
	long set_value = *((long *) var_val);

	DEBUGMSGTL(("strMIB", "write_strMaxpsz entering action=%d...  \n", action));
	if ((StorageTmp = strMIBStorage) == NULL)
		return SNMP_ERR_NOSUCHNAME;	/* remove if you support creation here */
	switch (action) {
	case RESERVE1:
		if (var_val_type != ASN_INTEGER) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strMaxpsz not ASN_INTEGER\n");
			return SNMP_ERR_WRONGTYPE;
		}
		if (var_val_len > sizeof(int32_t)) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strMaxpsz: bad length\n");
			return SNMP_ERR_WRONGLENGTH;
		}
		/* Note: default value 4096 */
		/* Note: ranges 0..2147482648 */
		if ((0 > set_value || set_value > 2147482648)) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strMaxpsz: bad value\n");
			return SNMP_ERR_WRONGVALUE;
		}
		break;
	case RESERVE2:		/* memory reseveration, final preparation... */
		break;
	case ACTION:		/* The variable has been stored in set_value for you to use, and you have just been asked to do something with it.  Note that anything done here must be reversable in
				   the UNDO case */
		old_value = StorageTmp->strMaxpsz;
		StorageTmp->strMaxpsz = set_value;
		break;
	case COMMIT:		/* Things are working well, so it's now safe to make the change permanently.  Make sure that anything done here can't fail! */
	{
		FILE *file;
		int value = StorageTmp->strMaxpsz;

		if ((file = fopen("/proc/sys/streams/maxpsz", "w")) == NULL || fprintf(file, "%d\n", value) < 0) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strMaxpsz: commit failed\n");
			return SNMP_ERR_COMMITFAILED;
		}
		if (file)
			fclose(file);
		break;
	}
	case UNDO:		/* Back out any changes made in the ACTION case */
		StorageTmp->strMaxpsz = old_value;
		/* fall through */
	case FREE:		/* Release any resources that have been allocated */
		break;
	}
	return SNMP_ERR_NOERROR;
}

/**
 * @fn int write_strMinpsz(int action, u_char *var_val, u_char var_val_type, size_t var_val_len, u_char *statP, oid *name, size_t name_len)
 * @param action the stage of the SET operation.
 * @param var_val pointer to the varbind value.
 * @param var_val_type the ASN type.
 * @param var_val_len the length of the varbind value.
 * @param statP static pointer.
 * @param name the varbind OID.
 * @param name_len number of elements in OID.
 * @brief Scalar write routine.
 */
int
write_strMinpsz(int action, u_char *var_val, u_char var_val_type, size_t var_val_len, u_char *statP, oid * name, size_t name_len)
{
	static long old_value;
	struct strMIB_data *StorageTmp = NULL;
	long set_value = *((long *) var_val);

	DEBUGMSGTL(("strMIB", "write_strMinpsz entering action=%d...  \n", action));
	if ((StorageTmp = strMIBStorage) == NULL)
		return SNMP_ERR_NOSUCHNAME;	/* remove if you support creation here */
	switch (action) {
	case RESERVE1:
		if (var_val_type != ASN_INTEGER) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strMinpsz not ASN_INTEGER\n");
			return SNMP_ERR_WRONGTYPE;
		}
		if (var_val_len > sizeof(int32_t)) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strMinpsz: bad length\n");
			return SNMP_ERR_WRONGLENGTH;
		}
		/* Note: default value 0 */
		/* Note: ranges 0..2147482648 */
		if ((0 > set_value || set_value > 2147482648)) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strMinpsz: bad value\n");
			return SNMP_ERR_WRONGVALUE;
		}
		break;
	case RESERVE2:		/* memory reseveration, final preparation... */
		break;
	case ACTION:		/* The variable has been stored in set_value for you to use, and you have just been asked to do something with it.  Note that anything done here must be reversable in
				   the UNDO case */
		old_value = StorageTmp->strMinpsz;
		StorageTmp->strMinpsz = set_value;
		break;
	case COMMIT:		/* Things are working well, so it's now safe to make the change permanently.  Make sure that anything done here can't fail! */
	{
		FILE *file;
		int value = StorageTmp->strMinpsz;

		if ((file = fopen("/proc/sys/streams/minpsz", "w")) == NULL || fprintf(file, "%d\n", value) < 0) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strMinpsz: commit failed\n");
			return SNMP_ERR_COMMITFAILED;
		}
		if (file)
			fclose(file);
		break;
	}
	case UNDO:		/* Back out any changes made in the ACTION case */
		StorageTmp->strMinpsz = old_value;
		/* fall through */
	case FREE:		/* Release any resources that have been allocated */
		break;
	}
	return SNMP_ERR_NOERROR;
}

/**
 * @fn int write_strReuseFmodsw(int action, u_char *var_val, u_char var_val_type, size_t var_val_len, u_char *statP, oid *name, size_t name_len)
 * @param action the stage of the SET operation.
 * @param var_val pointer to the varbind value.
 * @param var_val_type the ASN type.
 * @param var_val_len the length of the varbind value.
 * @param statP static pointer.
 * @param name the varbind OID.
 * @param name_len number of elements in OID.
 * @brief Scalar write routine.
 */
int
write_strReuseFmodsw(int action, u_char *var_val, u_char var_val_type, size_t var_val_len, u_char *statP, oid * name, size_t name_len)
{
	static long old_value;
	struct strMIB_data *StorageTmp = NULL;
	long set_value = *((long *) var_val);

	DEBUGMSGTL(("strMIB", "write_strReuseFmodsw entering action=%d...  \n", action));
	if ((StorageTmp = strMIBStorage) == NULL)
		return SNMP_ERR_NOSUCHNAME;	/* remove if you support creation here */
	switch (action) {
	case RESERVE1:
		if (var_val_type != ASN_INTEGER) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strReuseFmodsw not ASN_INTEGER\n");
			return SNMP_ERR_WRONGTYPE;
		}
		if (var_val_len > sizeof(int32_t)) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strReuseFmodsw: bad length\n");
			return SNMP_ERR_WRONGLENGTH;
		}
		/* Note: default value 2 */
		switch (set_value) {
		case TV_TRUE:
		case TV_FALSE:
			break;
		default:
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strReuseFmodsw: bad value\n");
			return SNMP_ERR_WRONGVALUE;
		}
		break;
	case RESERVE2:		/* memory reseveration, final preparation... */
		break;
	case ACTION:		/* The variable has been stored in set_value for you to use, and you have just been asked to do something with it.  Note that anything done here must be reversable in
				   the UNDO case */
		old_value = StorageTmp->strReuseFmodsw;
		StorageTmp->strReuseFmodsw = set_value;
		break;
	case COMMIT:		/* Things are working well, so it's now safe to make the change permanently.  Make sure that anything done here can't fail! */
	{
		FILE *file;
		int value = (StorageTmp->strReuseFmodsw == TV_TRUE) ? 1 : 0;

		if ((file = fopen("/proc/sys/streams/reuse_fmodsw", "w")) == NULL || fprintf(file, "%d\n", value) < 0) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strReuseFmodsw: commit failed\n");
			return SNMP_ERR_COMMITFAILED;
		}
		if (file)
			fclose(file);
		break;
	}
	case UNDO:		/* Back out any changes made in the ACTION case */
		StorageTmp->strReuseFmodsw = old_value;
		/* fall through */
	case FREE:		/* Release any resources that have been allocated */
		break;
	}
	return SNMP_ERR_NOERROR;
}

/**
 * @fn int write_strRtime(int action, u_char *var_val, u_char var_val_type, size_t var_val_len, u_char *statP, oid *name, size_t name_len)
 * @param action the stage of the SET operation.
 * @param var_val pointer to the varbind value.
 * @param var_val_type the ASN type.
 * @param var_val_len the length of the varbind value.
 * @param statP static pointer.
 * @param name the varbind OID.
 * @param name_len number of elements in OID.
 * @brief Scalar write routine.
 */
int
write_strRtime(int action, u_char *var_val, u_char var_val_type, size_t var_val_len, u_char *statP, oid * name, size_t name_len)
{
	static long old_value;
	struct strMIB_data *StorageTmp = NULL;
	long set_value = *((long *) var_val);

	DEBUGMSGTL(("strMIB", "write_strRtime entering action=%d...  \n", action));
	if ((StorageTmp = strMIBStorage) == NULL)
		return SNMP_ERR_NOSUCHNAME;	/* remove if you support creation here */
	switch (action) {
	case RESERVE1:
		if (var_val_type != ASN_INTEGER) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strRtime not ASN_INTEGER\n");
			return SNMP_ERR_WRONGTYPE;
		}
		if (var_val_len > sizeof(int32_t)) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strRtime: bad length\n");
			return SNMP_ERR_WRONGLENGTH;
		}
		/* Note: default value 1 */
		/* Note: ranges 0..2147483647 */
		if ((0 > set_value || set_value > 2147483647)) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strRtime: bad value\n");
			return SNMP_ERR_WRONGVALUE;
		}
		break;
	case RESERVE2:		/* memory reseveration, final preparation... */
		break;
	case ACTION:		/* The variable has been stored in set_value for you to use, and you have just been asked to do something with it.  Note that anything done here must be reversable in
				   the UNDO case */
		old_value = StorageTmp->strRtime;
		StorageTmp->strRtime = set_value;
		break;
	case COMMIT:		/* Things are working well, so it's now safe to make the change permanently.  Make sure that anything done here can't fail! */
	{
		FILE *file;
		ulong value = StorageTmp->strRtime * 10;	/* centiseconds to milliseconds */

		if ((file = fopen("/proc/sys/streams/rtime", "w")) == NULL || fprintf(file, "%lu\n", value) < 0) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strRtime: commit failed\n");
			return SNMP_ERR_COMMITFAILED;
		}
		if (file)
			fclose(file);
		break;
	}
	case UNDO:		/* Back out any changes made in the ACTION case */
		StorageTmp->strRtime = old_value;
		/* fall through */
	case FREE:		/* Release any resources that have been allocated */
		break;
	}
	return SNMP_ERR_NOERROR;
}

/**
 * @fn int write_strStrhold(int action, u_char *var_val, u_char var_val_type, size_t var_val_len, u_char *statP, oid *name, size_t name_len)
 * @param action the stage of the SET operation.
 * @param var_val pointer to the varbind value.
 * @param var_val_type the ASN type.
 * @param var_val_len the length of the varbind value.
 * @param statP static pointer.
 * @param name the varbind OID.
 * @param name_len number of elements in OID.
 * @brief Scalar write routine.
 */
int
write_strStrhold(int action, u_char *var_val, u_char var_val_type, size_t var_val_len, u_char *statP, oid * name, size_t name_len)
{
	static long old_value;
	struct strMIB_data *StorageTmp = NULL;
	long set_value = *((long *) var_val);

	DEBUGMSGTL(("strMIB", "write_strStrhold entering action=%d...  \n", action));
	if ((StorageTmp = strMIBStorage) == NULL)
		return SNMP_ERR_NOSUCHNAME;	/* remove if you support creation here */
	switch (action) {
	case RESERVE1:
		if (var_val_type != ASN_INTEGER) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strStrhold not ASN_INTEGER\n");
			return SNMP_ERR_WRONGTYPE;
		}
		if (var_val_len > sizeof(int32_t)) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strStrhold: bad length\n");
			return SNMP_ERR_WRONGLENGTH;
		}
		/* Note: default value 2 */
		switch (set_value) {
		case TV_TRUE:
		case TV_FALSE:
			break;
		default:
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strStrhold: bad value\n");
			return SNMP_ERR_WRONGVALUE;
		}
		break;
	case RESERVE2:		/* memory reseveration, final preparation... */
		break;
	case ACTION:		/* The variable has been stored in set_value for you to use, and you have just been asked to do something with it.  Note that anything done here must be reversable in
				   the UNDO case */
		old_value = StorageTmp->strStrhold;
		StorageTmp->strStrhold = set_value;
		break;
	case COMMIT:		/* Things are working well, so it's now safe to make the change permanently.  Make sure that anything done here can't fail! */
	{
		FILE *file;
		int value = (StorageTmp->strStrhold == TV_TRUE) ? 1 : 0;

		if ((file = fopen("/proc/sys/streams/strhold", "w")) == NULL || fprintf(file, "%d\n", value) < 0) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strStrhold: commit failed\n");
			return SNMP_ERR_COMMITFAILED;
		}
		if (file)
			fclose(file);
		break;
	}
	case UNDO:		/* Back out any changes made in the ACTION case */
		StorageTmp->strStrhold = old_value;
		/* fall through */
	case FREE:		/* Release any resources that have been allocated */
		break;
	}
	return SNMP_ERR_NOERROR;
}

/**
 * @fn int write_strStrctlsz(int action, u_char *var_val, u_char var_val_type, size_t var_val_len, u_char *statP, oid *name, size_t name_len)
 * @param action the stage of the SET operation.
 * @param var_val pointer to the varbind value.
 * @param var_val_type the ASN type.
 * @param var_val_len the length of the varbind value.
 * @param statP static pointer.
 * @param name the varbind OID.
 * @param name_len number of elements in OID.
 * @brief Scalar write routine.
 */
int
write_strStrctlsz(int action, u_char *var_val, u_char var_val_type, size_t var_val_len, u_char *statP, oid * name, size_t name_len)
{
	static ulong old_value;
	struct strMIB_data *StorageTmp = NULL;
	ulong set_value = *((ulong *) var_val);

	DEBUGMSGTL(("strMIB", "write_strStrctlsz entering action=%d...  \n", action));
	if ((StorageTmp = strMIBStorage) == NULL)
		return SNMP_ERR_NOSUCHNAME;	/* remove if you support creation here */
	switch (action) {
	case RESERVE1:
		if (var_val_type != ASN_UNSIGNED) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strStrctlsz not ASN_UNSIGNED\n");
			return SNMP_ERR_WRONGTYPE;
		}
		if (var_val_len > sizeof(uint32_t)) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strStrctlsz: bad length\n");
			return SNMP_ERR_WRONGLENGTH;
		}
		/* Note: default value 4096 */
		/* Note: ranges 1024..262144 */
		if ((1024 > set_value || set_value > 262144)) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strStrctlsz: bad value\n");
			return SNMP_ERR_WRONGVALUE;
		}
		break;
	case RESERVE2:		/* memory reseveration, final preparation... */
		break;
	case ACTION:		/* The variable has been stored in set_value for you to use, and you have just been asked to do something with it.  Note that anything done here must be reversable in
				   the UNDO case */
		old_value = StorageTmp->strStrctlsz;
		StorageTmp->strStrctlsz = set_value;
		break;
	case COMMIT:		/* Things are working well, so it's now safe to make the change permanently.  Make sure that anything done here can't fail! */
	{
		FILE *file;
		ulong value = StorageTmp->strStrctlsz;

		if ((file = fopen("/proc/sys/streams/strhold", "w")) == NULL || fprintf(file, "%lu\n", value) < 0) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strStrctlsz: commit failed\n");
			return SNMP_ERR_COMMITFAILED;
		}
		if (file)
			fclose(file);
		break;
	}
	case UNDO:		/* Back out any changes made in the ACTION case */
		StorageTmp->strStrctlsz = old_value;
		/* fall through */
	case FREE:		/* Release any resources that have been allocated */
		break;
	}
	return SNMP_ERR_NOERROR;
}

/**
 * @fn int write_strStrmsgsz(int action, u_char *var_val, u_char var_val_type, size_t var_val_len, u_char *statP, oid *name, size_t name_len)
 * @param action the stage of the SET operation.
 * @param var_val pointer to the varbind value.
 * @param var_val_type the ASN type.
 * @param var_val_len the length of the varbind value.
 * @param statP static pointer.
 * @param name the varbind OID.
 * @param name_len number of elements in OID.
 * @brief Scalar write routine.
 */
int
write_strStrmsgsz(int action, u_char *var_val, u_char var_val_type, size_t var_val_len, u_char *statP, oid * name, size_t name_len)
{
	static ulong old_value;
	struct strMIB_data *StorageTmp = NULL;
	ulong set_value = *((ulong *) var_val);

	DEBUGMSGTL(("strMIB", "write_strStrmsgsz entering action=%d...  \n", action));
	if ((StorageTmp = strMIBStorage) == NULL)
		return SNMP_ERR_NOSUCHNAME;	/* remove if you support creation here */
	switch (action) {
	case RESERVE1:
		if (var_val_type != ASN_UNSIGNED) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strStrmsgsz not ASN_UNSIGNED\n");
			return SNMP_ERR_WRONGTYPE;
		}
		if (var_val_len > sizeof(uint32_t)) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strStrmsgsz: bad length\n");
			return SNMP_ERR_WRONGLENGTH;
		}
		/* Note: default value 262144 */
		/* Note: ranges 1024..262144 */
		if ((1024 > set_value || set_value > 262144)) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strStrmsgsz: bad value\n");
			return SNMP_ERR_WRONGVALUE;
		}
		break;
	case RESERVE2:		/* memory reseveration, final preparation... */
		break;
	case ACTION:		/* The variable has been stored in set_value for you to use, and you have just been asked to do something with it.  Note that anything done here must be reversable in
				   the UNDO case */
		old_value = StorageTmp->strStrmsgsz;
		StorageTmp->strStrmsgsz = set_value;
		break;
	case COMMIT:		/* Things are working well, so it's now safe to make the change permanently.  Make sure that anything done here can't fail! */
	{
		FILE *file;
		ulong value = StorageTmp->strStrmsgsz;

		if ((file = fopen("/proc/sys/streams/strmsgsz", "w")) == NULL || fprintf(file, "%lu\n", value) < 0) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strStrmsgsz: commit failed\n");
			return SNMP_ERR_COMMITFAILED;
		}
		if (file)
			fclose(file);
		break;
	}
	case UNDO:		/* Back out any changes made in the ACTION case */
		StorageTmp->strStrmsgsz = old_value;
		/* fall through */
	case FREE:		/* Release any resources that have been allocated */
		break;
	}
	return SNMP_ERR_NOERROR;
}

/**
 * @fn int write_strStrthresh(int action, u_char *var_val, u_char var_val_type, size_t var_val_len, u_char *statP, oid *name, size_t name_len)
 * @param action the stage of the SET operation.
 * @param var_val pointer to the varbind value.
 * @param var_val_type the ASN type.
 * @param var_val_len the length of the varbind value.
 * @param statP static pointer.
 * @param name the varbind OID.
 * @param name_len number of elements in OID.
 * @brief Scalar write routine.
 */
int
write_strStrthresh(int action, u_char *var_val, u_char var_val_type, size_t var_val_len, u_char *statP, oid * name, size_t name_len)
{
	static ulong old_value;
	struct strMIB_data *StorageTmp = NULL;
	ulong set_value = *((ulong *) var_val);

	DEBUGMSGTL(("strMIB", "write_strStrthresh entering action=%d...  \n", action));
	if ((StorageTmp = strMIBStorage) == NULL)
		return SNMP_ERR_NOSUCHNAME;	/* remove if you support creation here */
	switch (action) {
	case RESERVE1:
		if (var_val_type != ASN_UNSIGNED) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strStrthresh not ASN_UNSIGNED\n");
			return SNMP_ERR_WRONGTYPE;
		}
		if (var_val_len > sizeof(uint32_t)) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strStrthresh: bad length\n");
			return SNMP_ERR_WRONGLENGTH;
		}
		/* Note: default value 0 */
		break;
	case RESERVE2:		/* memory reseveration, final preparation... */
		break;
	case ACTION:		/* The variable has been stored in set_value for you to use, and you have just been asked to do something with it.  Note that anything done here must be reversable in
				   the UNDO case */
		old_value = StorageTmp->strStrthresh;
		StorageTmp->strStrthresh = set_value;
		break;
	case COMMIT:		/* Things are working well, so it's now safe to make the change permanently.  Make sure that anything done here can't fail! */
	{
		FILE *file;
		ulong value = StorageTmp->strStrthresh;

		if ((file = fopen("/proc/sys/streams/strthresh", "w")) == NULL || fprintf(file, "%lu\n", value) < 0) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strStrthresh: commit failed\n");
			return SNMP_ERR_COMMITFAILED;
		}
		if (file)
			fclose(file);
		break;
	}
	case UNDO:		/* Back out any changes made in the ACTION case */
		StorageTmp->strStrthresh = old_value;
		/* fall through */
	case FREE:		/* Release any resources that have been allocated */
		break;
	}
	return SNMP_ERR_NOERROR;
}

/**
 * @fn int write_strLowthresh(int action, u_char *var_val, u_char var_val_type, size_t var_val_len, u_char *statP, oid *name, size_t name_len)
 * @param action the stage of the SET operation.
 * @param var_val pointer to the varbind value.
 * @param var_val_type the ASN type.
 * @param var_val_len the length of the varbind value.
 * @param statP static pointer.
 * @param name the varbind OID.
 * @param name_len number of elements in OID.
 * @brief Scalar write routine.
 */
int
write_strLowthresh(int action, u_char *var_val, u_char var_val_type, size_t var_val_len, u_char *statP, oid * name, size_t name_len)
{
	static ulong old_value;
	struct strMIB_data *StorageTmp = NULL;
	ulong set_value = *((ulong *) var_val);

	DEBUGMSGTL(("strMIB", "write_strLowthresh entering action=%d...  \n", action));
	if ((StorageTmp = strMIBStorage) == NULL)
		return SNMP_ERR_NOSUCHNAME;	/* remove if you support creation here */
	switch (action) {
	case RESERVE1:
		if (var_val_type != ASN_UNSIGNED) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strLowthresh not ASN_UNSIGNED\n");
			return SNMP_ERR_WRONGTYPE;
		}
		if (var_val_len > sizeof(uint32_t)) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strLowthresh: bad length\n");
			return SNMP_ERR_WRONGLENGTH;
		}
		/* Note: default value 0 */
		break;
	case RESERVE2:		/* memory reseveration, final preparation... */
		break;
	case ACTION:		/* The variable has been stored in set_value for you to use, and you have just been asked to do something with it.  Note that anything done here must be reversable in
				   the UNDO case */
		old_value = StorageTmp->strLowthresh;
		StorageTmp->strLowthresh = set_value;
		break;
	case COMMIT:		/* Things are working well, so it's now safe to make the change permanently.  Make sure that anything done here can't fail! */
		break;
	case UNDO:		/* Back out any changes made in the ACTION case */
		StorageTmp->strLowthresh = old_value;
		/* fall through */
	case FREE:		/* Release any resources that have been allocated */
		break;
	}
	return SNMP_ERR_NOERROR;
}

/**
 * @fn int write_strMedthresh(int action, u_char *var_val, u_char var_val_type, size_t var_val_len, u_char *statP, oid *name, size_t name_len)
 * @param action the stage of the SET operation.
 * @param var_val pointer to the varbind value.
 * @param var_val_type the ASN type.
 * @param var_val_len the length of the varbind value.
 * @param statP static pointer.
 * @param name the varbind OID.
 * @param name_len number of elements in OID.
 * @brief Scalar write routine.
 */
int
write_strMedthresh(int action, u_char *var_val, u_char var_val_type, size_t var_val_len, u_char *statP, oid * name, size_t name_len)
{
	static ulong old_value;
	struct strMIB_data *StorageTmp = NULL;
	ulong set_value = *((ulong *) var_val);

	DEBUGMSGTL(("strMIB", "write_strMedthresh entering action=%d...  \n", action));
	if ((StorageTmp = strMIBStorage) == NULL)
		return SNMP_ERR_NOSUCHNAME;	/* remove if you support creation here */
	switch (action) {
	case RESERVE1:
		if (var_val_type != ASN_UNSIGNED) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strMedthresh not ASN_UNSIGNED\n");
			return SNMP_ERR_WRONGTYPE;
		}
		if (var_val_len > sizeof(uint32_t)) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strMedthresh: bad length\n");
			return SNMP_ERR_WRONGLENGTH;
		}
		/* Note: default value 0 */
		break;
	case RESERVE2:		/* memory reseveration, final preparation... */
		break;
	case ACTION:		/* The variable has been stored in set_value for you to use, and you have just been asked to do something with it.  Note that anything done here must be reversable in
				   the UNDO case */
		old_value = StorageTmp->strMedthresh;
		StorageTmp->strMedthresh = set_value;
		break;
	case COMMIT:		/* Things are working well, so it's now safe to make the change permanently.  Make sure that anything done here can't fail! */
		break;
	case UNDO:		/* Back out any changes made in the ACTION case */
		StorageTmp->strMedthresh = old_value;
		/* fall through */
	case FREE:		/* Release any resources that have been allocated */
		break;
	}
	return SNMP_ERR_NOERROR;
}

/**
 * @fn int write_strIoctime(int action, u_char *var_val, u_char var_val_type, size_t var_val_len, u_char *statP, oid *name, size_t name_len)
 * @param action the stage of the SET operation.
 * @param var_val pointer to the varbind value.
 * @param var_val_type the ASN type.
 * @param var_val_len the length of the varbind value.
 * @param statP static pointer.
 * @param name the varbind OID.
 * @param name_len number of elements in OID.
 * @brief Scalar write routine.
 */
int
write_strIoctime(int action, u_char *var_val, u_char var_val_type, size_t var_val_len, u_char *statP, oid * name, size_t name_len)
{
	static long old_value;
	struct strMIB_data *StorageTmp = NULL;
	long set_value = *((long *) var_val);

	DEBUGMSGTL(("strMIB", "write_strIoctime entering action=%d...  \n", action));
	if ((StorageTmp = strMIBStorage) == NULL)
		return SNMP_ERR_NOSUCHNAME;	/* remove if you support creation here */
	switch (action) {
	case RESERVE1:
		if (var_val_type != ASN_INTEGER) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strIoctime not ASN_INTEGER\n");
			return SNMP_ERR_WRONGTYPE;
		}
		if (var_val_len > sizeof(int32_t)) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strIoctime: bad length\n");
			return SNMP_ERR_WRONGLENGTH;
		}
		/* Note: default value 1500 */
		/* Note: ranges 0..2147483647 */
		if ((0 > set_value || set_value > 2147483647)) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strIoctime: bad value\n");
			return SNMP_ERR_WRONGVALUE;
		}
		break;
	case RESERVE2:		/* memory reseveration, final preparation... */
		break;
	case ACTION:		/* The variable has been stored in set_value for you to use, and you have just been asked to do something with it.  Note that anything done here must be reversable in
				   the UNDO case */
		old_value = StorageTmp->strIoctime;
		StorageTmp->strIoctime = set_value;
		break;
	case COMMIT:		/* Things are working well, so it's now safe to make the change permanently.  Make sure that anything done here can't fail! */
	{
		FILE *file;
		ulong value = StorageTmp->strIoctime * 10;	/* centiseconds to milliseconds */

		if ((file = fopen("/proc/sys/streams/ioctime", "w")) == NULL || fprintf(file, "%lu\n", value) < 0) {
			snmp_log(MY_FACILITY(LOG_NOTICE), "write to strIoctime: commit failed\n");
			return SNMP_ERR_COMMITFAILED;
		}
		if (file)
			fclose(file);
		break;
	}
	case UNDO:		/* Back out any changes made in the ACTION case */
		StorageTmp->strIoctime = old_value;
		/* fall through */
	case FREE:		/* Release any resources that have been allocated */
		break;
	}
	return SNMP_ERR_NOERROR;
}

/**
 * @fn int strModTable_consistent(struct strModTable_data *thedata)
 * @param thedata the row data to check for consistency.
 * @brief check the internal consistency of a table row.
 *
 * This function checks the internal consistency of a table row for the strModTable table.  If the
 * table row is internally consistent, then this function returns SNMP_ERR_NOERROR, otherwise the
 * function returns an SNMP error code and it will not be possible to activate the row until the
 * row's internal consistency is corrected.  This function might use a 'test' operation against the
 * driver to ensure that the commit phase will succeed.
 */
int
strModTable_consistent(struct strModTable_data *thedata)
{
	/* XXX: check row consistency return SNMP_ERR_NOERROR if consistent, or an SNMP error code if not. */
	/* Creating a row in the strModTable has the effect of attempting to verify the module by loading it.  We could use the sad(4) driver's ability to verify a module list; however, it does not
	   work for drivers.  So, we use the sc(4) modules' ability to tune a module or driver, but don't really tune any of it (just for the side-effect of demand loading). */
	int fd;
	struct sc_tlist sc;
	struct sc_tune t[4];

	if ((fd = open("/dev/streams/clone/sad", O_RDWR)) < 0) {
		snmp_log_perror("strModTable_consistent: open(/dev/streams/clone/sad)");
		return SNMP_ERR_GENERR;
	}
	if (ioctl(fd, I_PUSH, "sc") < 0) {
		snmp_log_perror("strModTable_consistent: ioctl(I_PUSH,\"sc\")");
		close(fd);
		return SNMP_ERR_GENERR;
	}
	memset(&sc, 0, sizeof(sc));
	sc.sc_major = 0;
	memcpy(sc.sc_name, thedata->strModName, thedata->strModNameLen);
	sc.sc_ntune = 4;
	sc.sc_tlist = t;
	memset(t, 0, 4 * sizeof(t[0]));
	if (ioctl(fd, SC_IOC_TUNE, &sc) < 0) {
		snmp_log_perror("strModTable_consistent: ioctl(SC_IOC_TUNE)");
		close(fd);
		return SNMP_ERR_INCONSISTENTNAME;
	}
	close(fd);
	return (SNMP_ERR_NOERROR);
}

/**
 * @fn int strModInfoTable_consistent(struct strModInfoTable_data *thedata)
 * @param thedata the row data to check for consistency.
 * @brief check the internal consistency of a table row.
 *
 * This function checks the internal consistency of a table row for the strModInfoTable table.  If the
 * table row is internally consistent, then this function returns SNMP_ERR_NOERROR, otherwise the
 * function returns an SNMP error code and it will not be possible to activate the row until the
 * row's internal consistency is corrected.  This function might use a 'test' operation against the
 * driver to ensure that the commit phase will succeed.
 */
int
strModInfoTable_consistent(struct strModInfoTable_data *thedata)
{
	/* XXX: check row consistency return SNMP_ERR_NOERROR if consistent, or an SNMP error code if not. */
	return (SNMP_ERR_NOERROR);
}

/**
 * @fn int strModStatTable_consistent(struct strModStatTable_data *thedata)
 * @param thedata the row data to check for consistency.
 * @brief check the internal consistency of a table row.
 *
 * This function checks the internal consistency of a table row for the strModStatTable table.  If the
 * table row is internally consistent, then this function returns SNMP_ERR_NOERROR, otherwise the
 * function returns an SNMP error code and it will not be possible to activate the row until the
 * row's internal consistency is corrected.  This function might use a 'test' operation against the
 * driver to ensure that the commit phase will succeed.
 */
int
strModStatTable_consistent(struct strModStatTable_data *thedata)
{
	/* XXX: check row consistency return SNMP_ERR_NOERROR if consistent, or an SNMP error code if not. */
	return (SNMP_ERR_NOERROR);
}

/**
 * @fn int strApshTable_consistent(struct strApshTable_data *thedata)
 * @param thedata the row data to check for consistency.
 * @brief check the internal consistency of a table row.
 *
 * This function checks the internal consistency of a table row for the strApshTable table.  If the
 * table row is internally consistent, then this function returns SNMP_ERR_NOERROR, otherwise the
 * function returns an SNMP error code and it will not be possible to activate the row until the
 * row's internal consistency is corrected.  This function might use a 'test' operation against the
 * driver to ensure that the commit phase will succeed.
 */
int
strApshTable_consistent(struct strApshTable_data *thedata)
{
	/* XXX: check row consistency return SNMP_ERR_NOERROR if consistent, or an SNMP error code if not. */
	return (SNMP_ERR_NOERROR);
}

/**
 * @fn int strStatsTable_consistent(struct strStatsTable_data *thedata)
 * @param thedata the row data to check for consistency.
 * @brief check the internal consistency of a table row.
 *
 * This function checks the internal consistency of a table row for the strStatsTable table.  If the
 * table row is internally consistent, then this function returns SNMP_ERR_NOERROR, otherwise the
 * function returns an SNMP error code and it will not be possible to activate the row until the
 * row's internal consistency is corrected.  This function might use a 'test' operation against the
 * driver to ensure that the commit phase will succeed.
 */
int
strStatsTable_consistent(struct strStatsTable_data *thedata)
{
	/* XXX: check row consistency return SNMP_ERR_NOERROR if consistent, or an SNMP error code if not. */
	return (SNMP_ERR_NOERROR);
}

/**
 * @fn int write_strModRowStatus(int action, u_char *var_val, u_char var_val_type, size_t var_val_len, u_char *statP, oid *name, size_t name_len)
 * @param action the stage of the SET operation.
 * @param var_val pointer to the varbind value.
 * @param var_val_type the ASN type.
 * @param var_val_len the length of the varbind value.
 * @param statP static pointer.
 * @param name the varbind OID.
 * @param name_len number of elements in OID.
 * @brief Row status write routine.
 */
int
write_strModRowStatus(int action, u_char *var_val, u_char var_val_type, size_t var_val_len, u_char *statP, oid * name, size_t name_len)
{
	struct strModTable_data *StorageTmp = NULL;
	static struct strModTable_data *StorageNew, *StorageDel;
	size_t newlen = name_len - 15;
	static int old_value;
	int set_value, ret;
	static struct variable_list *vars, *vp;

	DEBUGMSGTL(("strMIB", "write_strModRowStatus entering action=%d...  \n", action));
	StorageTmp = header_complex(strModTableStorage, NULL, &name[15], &newlen, 1, NULL, NULL);
	if (var_val_type != ASN_INTEGER || var_val == NULL) {
		snmp_log(MY_FACILITY(LOG_NOTICE), "write to strModRowStatus not ASN_INTEGER\n");
		return SNMP_ERR_WRONGTYPE;
	}
	set_value = *((long *) var_val);
	/* check legal range, and notReady is reserved for us, not a user */
	if (set_value < 1 || set_value > 6 || set_value == RS_NOTREADY) {
		snmp_log(MY_FACILITY(LOG_NOTICE), "write to strModRowStatus: bad value\n");
		return SNMP_ERR_WRONGVALUE;
	}
	switch (action) {
	case RESERVE1:
		/* stage one: test validity */
		StorageNew = StorageDel = NULL;
		vars = vp = NULL;
		switch (set_value) {
		case RS_CREATEANDGO:
		case RS_CREATEANDWAIT:
			if (StorageTmp != NULL)
				/* cannot create existing row */
				return SNMP_ERR_INCONSISTENTVALUE;
			break;
		case RS_NOTINSERVICE:
			if (StorageTmp == NULL)
				/* cannot change state of non-existent row */
				return SNMP_ERR_INCONSISTENTVALUE;
			if (StorageTmp->strModRowStatus != RS_NOTINSERVICE)
				/* simply cannot take out of service */
				return SNMP_ERR_INCONSISTENTVALUE;
			break;
		case RS_ACTIVE:
			if (StorageTmp == NULL)
				/* cannot change state of non-existent row */
				return SNMP_ERR_INCONSISTENTVALUE;
			if (StorageTmp->strModRowStatus == RS_NOTREADY)
				/* cannot change state of row that is not ready */
				return SNMP_ERR_INCONSISTENTVALUE;
			/* XXX: interaction with row storage type needed */
			if (set_value == RS_NOTINSERVICE && StorageTmp->strModTable_refs > 0)
				/* row is busy and cannot be moved to the RS_NOTINSERVICE state */
				return SNMP_ERR_INCONSISTENTVALUE;
			break;
		case RS_DESTROY:
			/* destroying existent or non-existent row is ok */
			if (StorageTmp == NULL)
				break;
			/* XXX: interaction with row storage type needed */
			if (StorageTmp->strModTable_refs > 0)
				/* row is busy and cannot be deleted */
				return SNMP_ERR_INCONSISTENTVALUE;
			/* We plain cannot destroy entries... */
			return SNMP_ERR_INCONSISTENTVALUE;
		case RS_NOTREADY:
			/* management station cannot set this, only agent can */
		default:
			return SNMP_ERR_INCONSISTENTVALUE;
		}
		break;
	case RESERVE2:
		/* memory reseveration, final preparation... */
		switch (set_value) {
		case RS_CREATEANDGO:
		case RS_CREATEANDWAIT:
			/* creation */
			vars = NULL;
			/* strModName */
			if ((vp = snmp_varlist_add_variable(&vars, NULL, 0, ASN_OCTET_STR, NULL, 0)) == NULL) {
				snmp_free_varbind(vars);
				return SNMP_ERR_RESOURCEUNAVAILABLE;
			}
			if (header_complex_parse_oid(&(name[15]), newlen, vars) != SNMPERR_SUCCESS) {
				snmp_free_varbind(vars);
				return SNMP_ERR_INCONSISTENTNAME;
			}
			vp = vars;
			/* strModName */
			/* Note: ranges 1..8 */
			if (vp->val_len > SPRINT_MAX_LEN || ((1 > vp->val_len || vp->val_len > 8))) {
				snmp_log(MY_FACILITY(LOG_NOTICE), "index strModName: bad length\n");
				snmp_free_varbind(vars);
				return SNMP_ERR_INCONSISTENTNAME;
			}
			vp = vp->next_variable;
			if ((StorageNew = strModTable_create()) == NULL) {
				snmp_free_varbind(vars);
				return SNMP_ERR_RESOURCEUNAVAILABLE;
			}
			vp = vars;
			memdup((void *) &StorageNew->strModName, vp->val.string, vp->val_len);
			StorageNew->strModNameLen = vp->val_len;
			vp = vp->next_variable;
			header_complex_add_data(&strModTableStorage, vars, StorageNew);	/* frees vars */
			break;
		case RS_DESTROY:
			/* destroy */
			if (StorageTmp != NULL) {
				/* exists, extract it for now */
				StorageDel = StorageTmp;
				strModTable_del(StorageDel);
			} else {
				StorageDel = NULL;
			}
			break;
		}
		break;
	case ACTION:
		/* The variable has been stored in set_value for you to use, and you have just been asked to do something with it.  Note that anything done here must be reversable in the UNDO case */
		switch (set_value) {
		case RS_CREATEANDGO:
			/* check that activation is possible */
			if ((ret = strModTable_consistent(StorageNew)) != SNMP_ERR_NOERROR)
				return (ret);
			break;
		case RS_CREATEANDWAIT:
			/* row does not have to be consistent */
			break;
		case RS_ACTIVE:
			old_value = StorageTmp->strModRowStatus;
			StorageTmp->strModRowStatus = set_value;
			if (old_value != RS_ACTIVE) {
				/* check that activation is possible */
				if ((ret = strModTable_consistent(StorageTmp)) != SNMP_ERR_NOERROR) {
					StorageTmp->strModRowStatus = old_value;
					return (ret);
				}
			}
			break;
		case RS_NOTINSERVICE:
			/* set the flag? */
			old_value = StorageTmp->strModRowStatus;
			StorageTmp->strModRowStatus = set_value;
			break;
		}
		break;
	case COMMIT:
		/* Things are working well, so it's now safe to make the change permanently.  Make sure that anything done here can't fail! */
		switch (set_value) {
		case RS_CREATEANDGO:
			/* row creation, set final state */
			/* XXX: commit creation to underlying device */
			/* XXX: activate with underlying device */
			StorageNew->strModRowStatus = RS_ACTIVE;
			/* There is no real COMMIT phase.  Successful verification means that the strModTable,
			   strModInfoTable and strModStatsTable must be marked for refresh. */
			strModTable_refresh = 1;
			strModInfoTable_refresh = 1;
			strModStatTable_refresh = 1;
			break;
		case RS_CREATEANDWAIT:
			/* row creation, set final state */
			StorageNew->strModRowStatus = RS_NOTINSERVICE;
			break;
		case RS_ACTIVE:
		case RS_NOTINSERVICE:
			/* state change already performed */
			if (old_value != set_value) {
				switch (set_value) {
				case RS_ACTIVE:
					/* XXX: activate with underlying device */
					/* already done in ACTION phase */
					break;
				case RS_NOTINSERVICE:
					/* XXX: deactivate with underlying device */
					/* not possible, precluded in RESERVE1 phase. */
					break;
				}
			}
			break;
		case RS_DESTROY:
			/* row deletion, free it its dead */
			strModTable_destroy(&StorageDel);
			/* strModTable_destroy() can handle NULL pointers. */
			break;
		}
		break;
	case UNDO:
		/* Back out any changes made in the ACTION case */
		switch (set_value) {
		case RS_ACTIVE:
		case RS_NOTINSERVICE:
			/* restore state */
			StorageTmp->strModRowStatus = old_value;
			break;
		case RS_CREATEANDGO:
			/* Unfortunately, the sad(4) driver does not have the ability to remove a module from the
			   system.  I think that I will add an SAD_RML input-output control to do exactly that. */
			break;
		}
		/* fall through */
	case FREE:
		/* Release any resources that have been allocated */
		switch (set_value) {
		case RS_CREATEANDGO:
		case RS_CREATEANDWAIT:
			/* creation */
			if (StorageNew) {
				strModTable_del(StorageNew);
				strModTable_destroy(&StorageNew);
			}
			break;
		case RS_DESTROY:
			/* row deletion, so add it again */
			if (StorageDel)
				strModTable_add(StorageDel);
			break;
		}
		break;
	}
	return SNMP_ERR_NOERROR;
}

/**
 * @fn int write_strApshRowStatus(int action, u_char *var_val, u_char var_val_type, size_t var_val_len, u_char *statP, oid *name, size_t name_len)
 * @param action the stage of the SET operation.
 * @param var_val pointer to the varbind value.
 * @param var_val_type the ASN type.
 * @param var_val_len the length of the varbind value.
 * @param statP static pointer.
 * @param name the varbind OID.
 * @param name_len number of elements in OID.
 * @brief Row status write routine.
 */
int
write_strApshRowStatus(int action, u_char *var_val, u_char var_val_type, size_t var_val_len, u_char *statP, oid * name, size_t name_len)
{
	struct strApshTable_data *StorageTmp = NULL;
	static struct strApshTable_data *StorageNew, *StorageDel;
	size_t newlen = name_len - 15;
	static int old_value;
	int set_value, ret;
	static struct variable_list *vars, *vp;

	DEBUGMSGTL(("strMIB", "write_strApshRowStatus entering action=%d...  \n", action));
	StorageTmp = header_complex(strApshTableStorage, NULL, &name[15], &newlen, 1, NULL, NULL);
	if (var_val_type != ASN_INTEGER || var_val == NULL) {
		snmp_log(MY_FACILITY(LOG_NOTICE), "write to strApshRowStatus not ASN_INTEGER\n");
		return SNMP_ERR_WRONGTYPE;
	}
	set_value = *((long *) var_val);
	/* check legal range, and notReady is reserved for us, not a user */
	if (set_value < 1 || set_value > 6 || set_value == RS_NOTREADY) {
		snmp_log(MY_FACILITY(LOG_NOTICE), "write to strApshRowStatus: bad value\n");
		return SNMP_ERR_WRONGVALUE;
	}
	switch (action) {
	case RESERVE1:
		/* stage one: test validity */
		StorageNew = StorageDel = NULL;
		vars = vp = NULL;
		switch (set_value) {
		case RS_CREATEANDGO:
		case RS_CREATEANDWAIT:
			if (StorageTmp != NULL)
				/* cannot create existing row */
				return SNMP_ERR_INCONSISTENTVALUE;
			break;
		case RS_ACTIVE:
		case RS_NOTINSERVICE:
			if (StorageTmp == NULL)
				/* cannot change state of non-existent row */
				return SNMP_ERR_INCONSISTENTVALUE;
			if (StorageTmp->strApshRowStatus == RS_NOTREADY)
				/* cannot change state of row that is not ready */
				return SNMP_ERR_INCONSISTENTVALUE;
			/* XXX: interaction with row storage type needed */
			if (set_value == RS_NOTINSERVICE && StorageTmp->strApshTable_refs > 0)
				/* row is busy and cannot be moved to the RS_NOTINSERVICE state */
				return SNMP_ERR_INCONSISTENTVALUE;
			break;
		case RS_DESTROY:
			/* destroying existent or non-existent row is ok */
			if (StorageTmp == NULL)
				break;
			/* XXX: interaction with row storage type needed */
			if (StorageTmp->strApshTable_refs > 0)
				/* row is busy and cannot be deleted */
				return SNMP_ERR_INCONSISTENTVALUE;
			break;
		case RS_NOTREADY:
			/* management station cannot set this, only agent can */
		default:
			return SNMP_ERR_INCONSISTENTVALUE;
		}
		break;
	case RESERVE2:
		/* memory reseveration, final preparation... */
		switch (set_value) {
		case RS_CREATEANDGO:
		case RS_CREATEANDWAIT:
			/* creation */
			vars = NULL;
			/* strModName */
			if ((vp = snmp_varlist_add_variable(&vars, NULL, 0, ASN_OCTET_STR, NULL, 0)) == NULL) {
				snmp_free_varbind(vars);
				return SNMP_ERR_RESOURCEUNAVAILABLE;
			}
			/* strApshMinor */
			if ((vp = snmp_varlist_add_variable(&vars, NULL, 0, ASN_UNSIGNED, NULL, 0)) == NULL) {
				snmp_free_varbind(vars);
				return SNMP_ERR_RESOURCEUNAVAILABLE;
			}
			/* strApshLastMinor */
			if ((vp = snmp_varlist_add_variable(&vars, NULL, 0, ASN_UNSIGNED, NULL, 0)) == NULL) {
				snmp_free_varbind(vars);
				return SNMP_ERR_RESOURCEUNAVAILABLE;
			}
			if (header_complex_parse_oid(&(name[15]), newlen, vars) != SNMPERR_SUCCESS) {
				snmp_free_varbind(vars);
				return SNMP_ERR_INCONSISTENTNAME;
			}
			vp = vars;
			/* strModName */
			/* Note: ranges 1..8 */
			if (vp->val_len > SPRINT_MAX_LEN || ((1 > vp->val_len || vp->val_len > 8))) {
				snmp_log(MY_FACILITY(LOG_NOTICE), "index strModName: bad length\n");
				snmp_free_varbind(vars);
				return SNMP_ERR_INCONSISTENTNAME;
			}
			vp = vp->next_variable;
			/* strApshMinor */
			if (vp->val_len > sizeof(uint32_t)) {
				snmp_log(MY_FACILITY(LOG_NOTICE), "index strApshMinor: bad length\n");
				snmp_free_varbind(vars);
				return SNMP_ERR_INCONSISTENTNAME;
			}
			/* Note: ranges 0..16383 2147483647..2147483647 */
			if ((0 > (ulong) *vp->val.integer || (ulong) *vp->val.integer > 16383) && (ulong) *vp->val.integer != 2147483647) {
				snmp_log(MY_FACILITY(LOG_NOTICE), "index strApshMinor: bad value\n");
				snmp_free_varbind(vars);
				return SNMP_ERR_INCONSISTENTNAME;
			}
			vp = vp->next_variable;
			/* strApshLastMinor */
			if (vp->val_len > sizeof(uint32_t)) {
				snmp_log(MY_FACILITY(LOG_NOTICE), "index strApshLastMinor: bad length\n");
				snmp_free_varbind(vars);
				return SNMP_ERR_INCONSISTENTNAME;
			}
			/* Note: ranges 0..16383 */
			if ((0 > (ulong) *vp->val.integer || (ulong) *vp->val.integer > 16383)) {
				snmp_log(MY_FACILITY(LOG_NOTICE), "index strApshLastMinor: bad value\n");
				snmp_free_varbind(vars);
				return SNMP_ERR_INCONSISTENTNAME;
			}
			vp = vp->next_variable;
			if ((StorageNew = strApshTable_create()) == NULL) {
				snmp_free_varbind(vars);
				return SNMP_ERR_RESOURCEUNAVAILABLE;
			}
			vp = vars;
			memdup((void *) &StorageNew->strModName, vp->val.string, vp->val_len);
			StorageNew->strModNameLen = vp->val_len;
			vp = vp->next_variable;
			StorageNew->strApshMinor = (ulong) *vp->val.integer;
			vp = vp->next_variable;
			StorageNew->strApshLastMinor = (ulong) *vp->val.integer;
			vp = vp->next_variable;
			header_complex_add_data(&strApshTableStorage, vars, StorageNew);	/* frees vars */
			break;
		case RS_DESTROY:
			/* destroy */
			if (StorageTmp != NULL) {
				/* exists, extract it for now */
				StorageDel = StorageTmp;
				strApshTable_del(StorageDel);
			} else {
				StorageDel = NULL;
			}
			break;
		}
		break;
	case ACTION:
		/* The variable has been stored in set_value for you to use, and you have just been asked to do something with it.  Note that anything done here must be reversable in the UNDO case */
		switch (set_value) {
		case RS_CREATEANDGO:
			/* check that activation is possible */
			if ((ret = strApshTable_consistent(StorageNew)) != SNMP_ERR_NOERROR)
				return (ret);
			break;
		case RS_CREATEANDWAIT:
			/* row does not have to be consistent */
			break;
		case RS_ACTIVE:
			old_value = StorageTmp->strApshRowStatus;
			StorageTmp->strApshRowStatus = set_value;
			if (old_value != RS_ACTIVE) {
				/* check that activation is possible */
				if ((ret = strApshTable_consistent(StorageTmp)) != SNMP_ERR_NOERROR) {
					StorageTmp->strApshRowStatus = old_value;
					return (ret);
				}
			}
			break;
		case RS_NOTINSERVICE:
			/* set the flag? */
			old_value = StorageTmp->strApshRowStatus;
			StorageTmp->strApshRowStatus = set_value;
			break;
		}
		break;
	case COMMIT:
		/* Things are working well, so it's now safe to make the change permanently.  Make sure that anything done here can't fail! */
		switch (set_value) {
		case RS_CREATEANDGO:
			/* row creation, set final state */
			/* XXX: commit creation to underlying device */
			/* XXX: activate with underlying device */
			StorageNew->strApshRowStatus = RS_ACTIVE;
			break;
		case RS_CREATEANDWAIT:
			/* row creation, set final state */
			StorageNew->strApshRowStatus = RS_NOTINSERVICE;
			break;
		case RS_ACTIVE:
		case RS_NOTINSERVICE:
			/* state change already performed */
			if (old_value != set_value) {
				switch (set_value) {
				case RS_ACTIVE:
					/* XXX: activate with underlying device */
					break;
				case RS_NOTINSERVICE:
					/* XXX: deactivate with underlying device */
					break;
				}
			}
			break;
		case RS_DESTROY:
			/* row deletion, free it its dead */
			strApshTable_destroy(&StorageDel);
			/* strApshTable_destroy() can handle NULL pointers. */
			break;
		}
		break;
	case UNDO:
		/* Back out any changes made in the ACTION case */
		switch (set_value) {
		case RS_ACTIVE:
		case RS_NOTINSERVICE:
			/* restore state */
			StorageTmp->strApshRowStatus = old_value;
			break;
		}
		/* fall through */
	case FREE:
		/* Release any resources that have been allocated */
		switch (set_value) {
		case RS_CREATEANDGO:
		case RS_CREATEANDWAIT:
			/* creation */
			if (StorageNew) {
				strApshTable_del(StorageNew);
				strApshTable_destroy(&StorageNew);
			}
			break;
		case RS_DESTROY:
			/* row deletion, so add it again */
			if (StorageDel)
				strApshTable_add(StorageDel);
			break;
		}
		break;
	}
	return SNMP_ERR_NOERROR;
}

/**
 * @fn void strMIB_loop_handler(int dummy)
 * @param sig signal number
 * @brief handle event loop interation.
 *
 * This function is registered so that, when operating as a module, snmpd will call it one per event
 * loop interation.  This function is called before the next requst is processed and after the
 * previous request is processed.  Two things are done here:  1) The file descriptor that is used to
 * synchronize the agent with (pseudo-)device drivers is closed.  (Another approach, instead of
 * closing each time, would be to restart a timer each time that a request is made (loop is
 * performed) and if it expires, close the file descriptor).  2) The request number is incremented.
 * Although a request is not generated for each loop of the snmp event loop, it is true that a new
 * request cannot be generated without performing a loop.  Therefore, the sa_request is not the
 * request number but it is a temporally unique identifier for a request.
 */
void
strMIB_loop_handler(int sig)
{
	DEBUGMSGTL(("strMIB", "strMIB_loop_handler: executing loop handler...  "));
	/* close files after each request */
	if (sa_fclose) {
		if (my_fd >= 0) {
			close(my_fd);
			my_fd = -1;
		}
	}
#if defined MASTER
	/* prepare for next request */
	sa_request++;
#endif				/* defined MASTER */
	if (external_signal_scheduled[sig] == 0)
		external_signal_scheduled[sig]--;
	if (strMIBold_signal_handler != NULL)
		(*strMIBold_signal_handler) (sig);
	DEBUGMSGTL(("strMIB", "done.\n"));
}

/**
 * @fn void strMIB_fd_handler(int fd, void *dummy)
 * @param fd file descriptor to read.
 * @param dummy client data passed to registration function (always NULL).
 * @brief handle read event on file descriptor.
 *
 * This read file descriptor handler is normally used for (pseudo-)device drivers that generate
 * statistical collection interval events, alarm events, or other operational measurement events, by
 * placing a message on the read queue of the "event handling" Stream.  Normally this routine
 * would adjust counts in some table or scalars, generate SNMP traps representing on-occurence
 * events, first and interval events, and alarm indications.
 */
void
strMIB_fd_handler(int fd, void *dummy)
{
	DEBUGMSGTL(("strMIB", "strMIB_fd_handler: executing fd handler...  "));
	/* XXX: place actions to handle my_fd here... */
	DEBUGMSGTL(("strMIB", "done.\n"));
	return;
}
