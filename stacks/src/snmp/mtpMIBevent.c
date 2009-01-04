/*****************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2009  Monavacom Limited <http://www.monavacom.com/>
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

#include <ucd-snmp/ucd-snmp-config.h>
#include <ucd-snmp/ucd-snmp-includes.h>
#include <ucd-snmp/ucd-snmp-agent-includes.h>
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
/* Many recent net-snmp UCD compatible headers do not declard header_generic. */
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

/*
 * This file is responsible for reading statistics from the MTP layer as well as reading event
 * indication that are used for alarm and operational measurement event generation.  Following is a
 * function for generating one of each of the notifications defined by this MIB module.
 */

/**
 * @fn int xxxx_inform(const struct variable_list *man_parms, const struct variable_list *opt_parms)
 * @param man_parms a variable list containing the mandatory parameters of the notification.
 * @param opt_parms a variable list containing the optional parameters of the notification.
 * @brief generates the xxxx notification.
 *
 * This function generates the xxxx notification.  It accepts the list of mandatory parameters and a
 * list of optional parameters.  The list of mandatory parameters is checked against the
 * specification.  The list of optional parameters is not.  This function returns
 * zero(SNMPERR_SUCCESS) on success, and a non-zero value otherwise.
 */
int
xxxx_inform(const struct variable_list *man_parms, const struct variable_list *opt_parms)
{
	struct variable_list *vars = NULL, **vp;
	static const oid snmpTrapOID_oid[] = { 1, 3, 6, 1, 6, 3, 1, 1, 4, 1, 0 };

	snmp_varlist_add_variable(&vars, snmpTrapOID_oid, sizeof(snmpTrapOID_oid) / sizeof(oid), ASN_OBJECT_ID, (u_char *) xxxx_oid, sizeof(xxxx_oid));
	/* splice on mandatory parameters list */
	if (man_parms) {
		for (vp = &vars->next_variable; *vp; vp = &(*vp)->next_variable);
		*vp = man_parms;
	}
	/* splice on optional parameters list */
	if (opt_parms) {
		for (vp = &vars->next_variable; *vp; vp = &(*vp)->next_variable);
		*vp = opt_parms;
	}
}
