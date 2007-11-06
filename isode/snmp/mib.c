/*****************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

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

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

#ident "@(#) $RCSfile$ $Name$($Revision$) $Date$"

static char const ident[] = "$RCSfile$ $Name$($Revision$) $Date$";

/* mib.c - MIB realization */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/snmp/RCS/mib.c,v 9.0 1992/06/16 12:38:11 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/snmp/RCS/mib.c,v 9.0 1992/06/16 12:38:11 isode Rel
 *
 * Contributed by NYSERNet Inc.  This work was partially supported by the
 * U.S. Defense Advanced Research Projects Agency and the Rome Air Development
 * Center of the U.S. Air Force Systems Command under contract number
 * F30602-88-C-0016.
 *
 *
 * Log: mib.c,v
 * Revision 9.0  1992/06/16  12:38:11  isode
 * Release 8.0
 *
 */

/*
 *				  NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */

#include <stdio.h>
#include "mib.h"
#include "sys.file.h"
#include <sys/time.h>

#ifdef SYS5
#ifndef L_SET
#define L_SET SEEK_SET
#endif
#ifdef sgi
#include <sys/sbd.h>
#define KmemSeek(fd, addr) lseek(fd, (addr) & ~K0BASE, SEEK_SET)
#else
#define KmemSeek(fd, addr) lseek(fd, addr, L_SET)
#endif
#endif

/*    DATA */

static int kd;
static int wd;

#ifdef MACH
#define nlist_item(s) {{ s }}
#else
#define nlist_item(s) { s }
#endif

struct nlist nl[] = {
#ifdef sgi
	nlist_item("arptab"),
	nlist_item("arptab_size"),
	nlist_item("icmpstat"),
	nlist_item("ifnet"),
	nlist_item("ipforwarding"),
	nlist_item("ipstat"),
	nlist_item("rthashsize"),
	nlist_item("rthost"),
	nlist_item("rtnet"),
	nlist_item("tcb"),
	nlist_item("tcpstat"),
	nlist_item("udb"),
	nlist_item("udpstat"),
	nlist_item("rtstat"),
	nlist_item("mbstat"),
#else
	nlist_item("_arptab"),
	nlist_item("_arptab_size"),
	nlist_item("_icmpstat"),
	nlist_item("_ifnet"),
#ifndef	SUNOS41
	nlist_item("_ipforwarding"),
#else
	nlist_item("_ip_forwarding"),
#endif
	nlist_item("_ipstat"),
	nlist_item("_rthashsize"),
	nlist_item("_rthost"),
	nlist_item("_rtnet"),
	nlist_item("_tcb"),
	nlist_item("_tcpstat"),
	nlist_item("_udb"),
	nlist_item("_udpstat"),
	nlist_item("_rtstat"),
	nlist_item("_mbstat"),
#ifdef	BSD44
	nlist_item("_radix_node_head"),
	nlist_item("_iso_systype"),
	nlist_item("_clnp_stat"),
	nlist_item("_esis_stat"),
#endif
#endif
	nlist_item(NULL)
};

struct timeval my_boottime;

OID nullSpecific = NULLOID;

/*  */

#ifdef __NeXT__
#define	VMUNIX	"/mach"
#elif defined(sgi) || defined(__sgi__)
#define VMUNIX "/unix"
#else
#define	VMUNIX	"/vmunix"
#endif

init_mib()
{
	register struct nlist *nz;

	if (nlist(VMUNIX, nl) == NOTOK)
		adios(VMUNIX, "unable to nlist");
	for (nz = nl; nz->n_name; nz++)
		if (nz->n_value == 0)
			advise(LLOG_EXCEPTIONS, NULLCP, "\"%s\" not in %s (warning)",
			       nz->n_name, VMUNIX);

	if ((kd = open("/dev/kmem", O_RDONLY)) == NOTOK)
		adios("/dev/kmem", "unable to read");
	wd = NOTOK;

	if ((nullSpecific = text2oid("0.0")) == NULLOID)
		adios(NULLCP, "text2oid (\"0.0\") failed!");
}

/*  */

fin_mib()
{
	register OT ot;

	for (ot = text2obj("ccitt"); ot; ot = ot->ot_next)
		if (ot->ot_status == OT_MANDATORY
		    && ot->ot_getfnx == o_generic && ot->ot_info == NULL)
			advise(LLOG_EXCEPTIONS, NULLCP,
			       "variable \"%s.0\" has no value (warning)", ot->ot_text);

	if (gettimeofday(&my_boottime, (struct timezone *) 0) == NOTOK) {
		advise(LLOG_EXCEPTIONS, "failed", "gettimeofday");
		bzero((char *) &my_boottime, sizeof my_boottime);
	}
}

/*  */

set_variable(name, newvalue)
	char *name, *newvalue;
{
	caddr_t value;
	register OT ot = text2obj(name);
	register OS os;

	if (ot == NULLOT) {
		advise(LLOG_EXCEPTIONS, NULLCP, "unknown object \"%s\"", name);
		return;
	}
	if (ot->ot_getfnx == NULLIFP) {
		advise(LLOG_EXCEPTIONS, NULLCP, "no getfnx for object \"%s\"", ot->ot_text);
		return;
	}
	if (ot->ot_getfnx != o_generic) {
		advise(LLOG_EXCEPTIONS, NULLCP,
		       "non-generic getfnx for object \"%s\"", ot->ot_text);
		return;
	}
	if ((os = ot->ot_syntax) == NULLOS) {
		advise(LLOG_EXCEPTIONS, NULLCP, "no syntax defined for object \"%s\"", ot->ot_text);
		return;
	}
	if ((*os->os_parse) (&value, newvalue) == NOTOK) {
		advise(LLOG_EXCEPTIONS, NULLCP,
		       "invalid value for variable \"%s.0\": \"%s\"", ot->ot_text, newvalue);
		return;
	}
	if (ot->ot_info) {
		(*os->os_free) (ot->ot_info);
		ot->ot_info = NULL;
	}
	ot->ot_info = value;
}

/*  */

int
getkmem(n, buffer, cc)
	struct nlist *n;
	caddr_t buffer;
	int cc;
{
	if (n->n_value == 0) {
		advise(LLOG_EXCEPTIONS, NULLCP, "\"%s\" not in %s", n->n_name, VMUNIX);
		return NOTOK;
	}

	if (KmemSeek(kd, (long) n->n_value) == NOTOK) {
		advise(LLOG_EXCEPTIONS, "failed", "lseek of 0x%x for \"%s\" in kmem",
		       (long) n->n_value, n->n_name);
		return NOTOK;
	}

	if (read(kd, buffer, cc) != cc) {
		advise(LLOG_EXCEPTIONS, "failed", "read of \"%s\" from kmem", n->n_name);
		return NOTOK;
	}

	return OK;
}

int
setkmem(n, buffer, cc)
	struct nlist *n;
	caddr_t buffer;
	int cc;
{
	if (chekmem(n) == NOTOK)
		return NOTOK;

	if (write(wd, buffer, cc) != cc) {
		advise(LLOG_EXCEPTIONS, "failed", "write of \"%s\" to kmem", n->n_name);
		return NOTOK;
	}

	return OK;
}

int
chekmem(n)
	struct nlist *n;
{
	if (n == NULL) {
		if (wd != NOTOK)
			(void) close(wd), wd = NOTOK;

		return OK;
	}

	if (n->n_value == 0) {
		advise(LLOG_EXCEPTIONS, NULLCP, "\"%s\" not in %s", n->n_name, VMUNIX);
		return NOTOK;
	}

	if (wd == NOTOK && (wd = open("/dev/kmem", O_RDWR)) == NOTOK)
		advise(LLOG_EXCEPTIONS, "/dev/kmem", "unable to read");

	if (KmemSeek(wd, (long) n->n_value) == NOTOK) {
		advise(LLOG_EXCEPTIONS, "failed", "lseek of 0x%x for \"%s\" in kmem",
		       (long) n->n_value, n->n_name);
		return NOTOK;
	}

	return OK;
}
